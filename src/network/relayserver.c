#include "network/relayserver.h"
#include "network/packet.h"

#include "libwebsockets.h"
#include "ws.h"

#include <stdio.h>

typedef struct lws_context lws_context;
typedef struct lws_ring lws_ring;
typedef struct morobox8_packet_reader morobox8_packet_reader;
typedef struct morobox8_packet_writer morobox8_packet_writer;
typedef enum ws_event ws_event;
typedef struct ws ws;
typedef struct ws_listen_options ws_listen_options;
typedef struct ws_client ws_client;

#define RELAYSERVER_RX_SIZE 1024
#define RING_DEPTH 4096
#define RELAYSERVER_TRUE 1
#define RELAYSERVER_FALSE 0
#define RELAYSERVER_BUFFER_SIZE 1024

typedef struct relayserver_packet
{
    morobox8_u8 data[RELAYSERVER_BUFFER_SIZE];
    size_t size;
} relayserver_packet;

typedef struct relayserver_vhd
{
    struct lws_context *context;
    struct lws_vhost *vhost;

    int *interrupted;
    int *options;
} relayserver_vhd;

struct relayserver_client_data;

/* Info about a connected client. */
typedef struct relayserver_client_data
{
    ws_client *client;
    lws_ring *receive_queue;
    /* Id. */
    morobox8_u32 user_id;
    /* Is pending. */
    int pending;
    /* Is the host. */
    int host;
    /* Pointer to next client. */
    struct relayserver_client_data *next;
} relayserver_client_data;

/** Session clients connect to. */
typedef struct relayserver_session
{
    /* List of pending clients. */
    relayserver_client_data pending_clients;
    /* Number of pending clients. */
    size_t num_pending_clients;
    /* List of connected clients. */
    relayserver_client_data clients;
    /* Number of connected clients. */
    size_t num_clients;
} relayserver_session;

static relayserver_session session = {
    .num_pending_clients = 0,
    .num_clients = 0};

/* Packet reader/writer. */
static morobox8_u8 reader_buffer[RELAYSERVER_BUFFER_SIZE];
static morobox8_u8 writer_buffer[RELAYSERVER_BUFFER_SIZE];
static morobox8_packet_reader packet_reader = {
    .buf = reader_buffer,
    .size = RELAYSERVER_BUFFER_SIZE,
    .offset = 0};
static morobox8_packet_writer packet_writer = {
    .buf = writer_buffer,
    .size = RELAYSERVER_BUFFER_SIZE,
    .offset = 0};

/* Initialize a new client. */
static int relayserver_client_init(relayserver_client_data *client)
{
    client->receive_queue = lws_ring_create(
        sizeof(relayserver_packet),
        RING_DEPTH,
        NULL);

    if (!client->receive_queue)
    {
        return RELAYSERVER_FALSE;
    }

    client->user_id = 0;
    client->pending = RELAYSERVER_TRUE;

    return RELAYSERVER_TRUE;
}

/* Add a pending client to the session. */
static void relayserver_session_add_pending_client(relayserver_client_data *client)
{
    session.pending_clients.next = client;
    session.num_pending_clients++;
}

/* Remove a pending client from the session. */
static int relayserver_session_remove_pending_client(relayserver_client_data *client)
{
    for (relayserver_client_data *it = &session.pending_clients; it != NULL; it = it->next)
    {
        if (it->next == client)
        {
            it->next = client->next;
            session.num_pending_clients--;
            return RELAYSERVER_TRUE;
        }
    }

    return RELAYSERVER_FALSE;
}

/* Send a packet to a client. */
static void relayserver_session_send_raw(relayserver_client_data *client, const morobox8_u8 *data, size_t size)
{
    ws_send(client->client, data, size);
}

/* Send a packet to all clients. */
static void relayserver_session_broadcast_raw(relayserver_client_data *except, const morobox8_u8 *data, size_t size)
{
    for (relayserver_client_data *it = session.clients.next; it != NULL; it = it->next)
    {
        if (it != except)
        {
            ws_send(it->client, data, size);
        }
    }
}

/* Send a packet to a client. */
static void relayserver_session_send_to(relayserver_client_data *client, const morobox8_u8 *data, size_t size)
{
    packet_writer.offset = 0;
    morobox8_packet_write_u8(&packet_writer, MOROBOX8_PACKET_DATA);
    memcpy(&packet_writer.buf[packet_writer.offset], data, size);
    ws_send(client->client, packet_writer.buf, packet_writer.offset + size);
}

/* Send a packet to all clients. */
static void relayserver_session_broadcast(relayserver_client_data *except, const morobox8_u8 *data, size_t size)
{
    packet_writer.offset = 0;
    morobox8_packet_write_u8(&packet_writer, MOROBOX8_PACKET_DATA);
    memcpy(&packet_writer.buf[packet_writer.offset], data, size);
    relayserver_session_broadcast_raw(except, packet_writer.buf, packet_writer.offset + size);
}

/* Notify everyone a client joined. */
static void relayserver_session_notify_joined(relayserver_client_data *client)
{
    packet_writer.offset = 0;
    morobox8_packet_write_u8(&packet_writer, MOROBOX8_PACKET_SESSION_JOINED);
    morobox8_packet_write_u8(&packet_writer, client->host);
    morobox8_packet_write_u32(&packet_writer, client->user_id);
    relayserver_session_broadcast_raw(NULL, packet_writer.buf, packet_writer.offset);
}

/* Notify everyone a client left. */
static void relayserver_session_notify_left(relayserver_client_data *client)
{
    packet_writer.offset = 0;
    morobox8_packet_write_u8(&packet_writer, MOROBOX8_PACKET_SESSION_LEFT);
    morobox8_packet_write_u32(&packet_writer, client->user_id);
    relayserver_session_broadcast_raw(NULL, packet_writer.buf, packet_writer.offset);
}

/* Add a client to the session. */
static void relayserver_session_add_client(relayserver_client_data *client)
{
    client->next = session.clients.next;
    session.clients.next = client;
    session.num_clients++;
    client->host = session.num_clients == 1;
    printf("user %d added\n", client->user_id);
    relayserver_session_notify_joined(client);
}

/* Remove a client from the session. */
static int relayserver_session_remove_client(relayserver_client_data *client)
{
    for (relayserver_client_data *it = &session.clients; it != NULL; it = it->next)
    {
        if (it->next == client)
        {
            it->next = client->next;
            session.num_clients--;
            printf("user %d removed\n", client->user_id);
            relayserver_session_notify_left(client);
            return RELAYSERVER_TRUE;
        }
    }

    return RELAYSERVER_FALSE;
}

static relayserver_client_data *relayserver_session_find_client(morobox8_u32 id)
{
    if (id == 0)
    {
        return NULL;
    }

    for (relayserver_client_data *it = session.clients.next; it != NULL; it = it->next)
    {
        if (it->user_id == id)
        {
            return it;
        }
    }

    return NULL;
}

/* Authenticate the client. */
static int relayserver_session_auth(relayserver_client_data *client)
{
    client->user_id = morobox8_packet_read_u32(&packet_reader);
    printf("user %d connected\n", client->user_id);
    return RELAYSERVER_TRUE;
}

/* Called when CREATE_SESSION packet is received. */
static int relayserver_session_create(relayserver_client_data *client)
{
    if (!relayserver_session_auth(client))
    {
        return RELAYSERVER_FALSE;
    }

    /* Client already in session */
    if (relayserver_session_find_client(client->user_id))
    {
        return RELAYSERVER_FALSE;
    }

    /* Notify success */
    packet_writer.offset = 0;
    morobox8_packet_write_u8(&packet_writer, MOROBOX8_PACKET_SESSION_CREATED);
    relayserver_session_send_raw(client, (void *)packet_writer.buf, packet_writer.offset);

    relayserver_session_remove_pending_client(client);
    relayserver_session_add_client(client);
    return RELAYSERVER_TRUE;
}

/* Called when JOIN_SESSION packet is received. */
static int relayserver_session_join(relayserver_client_data *client)
{
    if (!relayserver_session_auth(client))
    {
        return RELAYSERVER_FALSE;
    }

    /* Client already in session */
    if (relayserver_session_find_client(client->user_id))
    {
        return RELAYSERVER_FALSE;
    }

    relayserver_session_remove_pending_client(client);
    relayserver_session_add_client(client);
    return RELAYSERVER_TRUE;
}

#include <assert.h>
static int
relayserver_callback(ws_client *client, enum ws_event event, void *user)
{
    relayserver_client_data *client_data = (relayserver_client_data *)user;

    switch (event)
    {
    case LIBWS_EVENT_CONNECTED:
        lwsl_user("WS_EVENT_CONNECTED\n");
        if (!relayserver_client_init(client_data))
        {
            return 1;
        }

        client_data->client = client;
        relayserver_session_add_pending_client(client_data);
        break;
    case LIBWS_EVENT_RECEIVED:
    {
        size_t len = ws_receive(client, &reader_buffer, RELAYSERVER_BUFFER_SIZE);

        /* Read packet type and get remaining data */
        packet_reader.offset = 0;
        morobox8_u8 packet_type = morobox8_packet_read_u8(&packet_reader);
        relayserver_client_data *other_client = NULL;

        switch (packet_type)
        {
        case MOROBOX8_PACKET_CREATE_SESSION:
            if (!relayserver_session_create(client_data))
            {
                lwsl_err("failed to create session\n");
                return 1;
            }
            break;
        case MOROBOX8_PACKET_JOIN_SESSION:
            if (!relayserver_session_join(client_data))
            {
                lwsl_err("failed to join session\n");
                return 1;
            }
            break;
        case MOROBOX8_PACKET_LEAVE_SESSION:
            if (relayserver_session_remove_client(client_data))
            {
                lwsl_err("failed to remove client\n");
                return 1;
            }
            break;
        case MOROBOX8_PACKET_BROADCAST:
            relayserver_session_broadcast(
                client_data,
                packet_reader.buf + packet_reader.offset,
                len - packet_reader.offset);
            break;
        case MOROBOX8_PACKET_SEND_TO:
            other_client = relayserver_session_find_client(
                morobox8_packet_read_u32(&packet_reader));
            if (!other_client)
            {
                return 0;
            }

            relayserver_session_send_to(
                other_client,
                packet_reader.buf + packet_reader.offset,
                len - packet_reader.offset);
            break;
        case MOROBOX8_PACKET_KEEP_ALIVE:
            break;
        default:
            /* Should not happen */
            lwsl_user("unknown packet %d\n", packet_type);
            return 1;
        }
    }
    break;

    case LIBWS_EVENT_CLOSED:
        lwsl_user("WS_EVENT_CLOSED\n");
        if (client_data->pending)
        {
            relayserver_session_remove_pending_client(client_data);
        }
        else
        {
            relayserver_session_remove_client(client_data);
        }
        break;

    default:
        break;
    }

    return 0;
}

ws *relayserver_create(lws_context *ctxt, int port)
{
    printf("create relay server\n");
    ws_listen_options options;
    memset(&options, 0, sizeof(ws_listen_options));
    options.context = ctxt;
    options.port = port;
    options.callback = &relayserver_callback;
    options.per_client_data_size = sizeof(relayserver_client_data);
    ws *server = ws_listen(&options);
    if (!server)
    {
        fprintf(stderr, "failed to create relay server\n");
        return NULL;
    }

    printf("relay server started on port %d\n", ws_get_port(server));
    return server;
}
