#include "moronet8.h"
#include "network/session_state.h"
#include "network/packet.h"

#include "libwebsockets.h"
#include "ws.h"

#if HAVE_STRING_H
#include <string.h>
#endif

#include <stdio.h>

#include <pthread.h>

typedef struct lws_ring lws_ring;
typedef struct lws_context lws_context;
typedef struct lws_context_creation_info lws_context_creation_info;
typedef enum moronet8_session_state moronet8_session_state;
typedef struct moronet8_client moronet8_client;
typedef struct moronet8_hooks moronet8_hooks;
typedef struct moronet8_packet_reader moronet8_packet_reader;
typedef struct moronet8_packet_writer moronet8_packet_writer;
typedef enum ws_event ws_event;
typedef struct ws ws;
typedef struct ws_connect_options ws_connect_options;
typedef struct ws_client ws_client;

#define RING_DEPTH 10
#define MORONET8_TRUE 1
#define MORONET8_FALSE 0
#define MORONET8_BUFFER_SIZE 1024

typedef struct moronet8_packet
{
    moronet8_u8 data[MORONET8_BUFFER_SIZE];
    size_t size;
} moronet8_packet;

typedef struct moronet8_player
{
    lws_ring *wait_receive_queue;
    lws_ring *receive_queue;
    /* Queued packets to send to client. */
    lws_ring *wait_send_queue;
    lws_ring *send_queue;
} moronet8_player;

typedef struct moronet8_session
{
    pthread_t thread;
    pthread_mutex_t mut;
    moronet8_session_state state;
    moronet8_u32 user_id;
    int host;
    lws_context *context;
    ws *socket;
    ws_client *client;
    moronet8_player *player;
} moronet8_session;

/* There can be only one session at a time. */
static moronet8_session session = {
    .state = MORONET8_SESSION_CLOSED,
    .host = MORONET8_FALSE,
    .context = NULL,
    .client = NULL,
    .player = NULL};

/* Packet reader/writer. */
static moronet8_u8 writer_buffer[MORONET8_BUFFER_SIZE];
static moronet8_packet_reader packet_reader = {
    .buf = writer_buffer,
    .size = MORONET8_BUFFER_SIZE,
    .offset = 0};
static moronet8_packet_writer packet_writer = {
    .buf = writer_buffer,
    .size = MORONET8_BUFFER_SIZE,
    .offset = 0};

static int moronet8_player_init(moronet8_player *player)
{
    player->wait_receive_queue = lws_ring_create(
        sizeof(moronet8_packet),
        RING_DEPTH,
        NULL);

    if (!player->wait_receive_queue)
    {
        return MORONET8_FALSE;
    }

    player->receive_queue = lws_ring_create(
        sizeof(moronet8_packet),
        RING_DEPTH,
        NULL);

    if (!player->receive_queue)
    {
        lws_ring_destroy(player->wait_receive_queue);
        return MORONET8_FALSE;
    }

    player->wait_send_queue = lws_ring_create(
        sizeof(moronet8_packet),
        RING_DEPTH,
        NULL);

    if (!player->wait_send_queue)
    {
        lws_ring_destroy(player->wait_receive_queue);
        lws_ring_destroy(player->receive_queue);
        return MORONET8_FALSE;
    }

    player->send_queue = lws_ring_create(
        sizeof(moronet8_packet),
        RING_DEPTH,
        NULL);

    if (!player->send_queue)
    {
        lws_ring_destroy(player->wait_receive_queue);
        lws_ring_destroy(player->receive_queue);
        lws_ring_destroy(player->wait_send_queue);
        return MORONET8_FALSE;
    }

    return MORONET8_TRUE;
}

static void
moronet8_session_connected(void)
{
    printf("connected to server\n");
    packet_writer.offset = 0;
    moronet8_packet_write_u8(&packet_writer, session.host ? MORONET8_PACKET_CREATE_SESSION : MORONET8_PACKET_JOIN_SESSION);
    moronet8_packet_write_u32(&packet_writer, session.user_id);
    ws_send(session.client, packet_writer.buf, packet_writer.offset);
}

static void
moronet8_session_created(void)
{
    printf("session created\n");
}

static void
moronet8_session_joined(void)
{
    printf("session joined\n");
    session.state = session.host ? MORONET8_SESSION_HOSTING : MORONET8_SESSION_JOINED;
}

static void
moronet8_session_left(void)
{
    printf("session left\n");
}

static int moronet8_session_callback(ws_client *client, ws_event event, void *user)
{
    moronet8_player *player = (moronet8_player *)user;
    moronet8_packet packet;

    switch (event)
    {
    case LIBWS_EVENT_CONNECTED:
        lwsl_user("LIBWS_EVENT_CONNECTED\n");
        if (!moronet8_player_init(player))
        {
            return 1;
        }

        session.client = client;
        session.player = player;
        moronet8_session_connected();
        break;

    case LIBWS_EVENT_RECEIVED:
    {
        /* Copy incoming data to read buffer */
        size_t len = ws_receive(client, packet_writer.buf, MORONET8_BUFFER_SIZE);
        if (len > packet_writer.size)
        {
            lwsl_user("OOM: can't copy packet\n");
            return 1;
        }

        /* Read packet type and get remaining data */
        packet_reader.offset = 0;
        packet_reader.size = len;
        moronet8_u8 packet_type = moronet8_packet_read_u8(&packet_reader);

        switch (packet_type)
        {
        case MORONET8_PACKET_SESSION_CREATED:
            moronet8_session_created();
            break;
        case MORONET8_PACKET_SESSION_JOINED:
            moronet8_session_joined();
            break;
        case MORONET8_PACKET_SESSION_LEFT:
            moronet8_session_left();
            break;
        case MORONET8_PACKET_DATA:
            memcpy(
                packet.data,
                &packet_reader.buf[packet_reader.offset],
                packet_reader.size - packet_reader.offset);
            packet.size = packet_reader.size - packet_reader.offset;

            pthread_mutex_lock(&session.mut);
            lws_ring_insert(player->wait_receive_queue, &packet, 1);
            pthread_mutex_unlock(&session.mut);
            break;
        case MORONET8_PACKET_KEEP_ALIVE:
            break;
        default:
            /* Should not happen */
            lwsl_err("unknown packet %d\n", packet_type);
            return 1;
        }
    }
    break;

    case LIBWS_EVENT_CLOSED:
        lwsl_user("LIBWS_EVENT_CLOSED\n");
        session.client = NULL;
        session.player = NULL;
        break;

    default:
        break;
    }

    return 0;
}

static void moronet8_session_dispatch_send(moronet8_session *session)
{
    if (!session->player)
    {
        return;
    }

    const moronet8_packet *pmsg = lws_ring_get_element(session->player->send_queue, 0);
    if (!pmsg)
    {
        return;
    }

    ws_send(session->client, pmsg->data, pmsg->size);
    lws_ring_consume(session->player->send_queue, NULL, NULL, 1);
}

static void moronet8_session_move_queue(moronet8_session *session, lws_ring *from, lws_ring *to)
{
    const void *pmsg = lws_ring_get_element(from, 0);
    if (!pmsg)
    {
        return;
    }

    lws_ring_insert(to, pmsg, 1);
    lws_ring_consume(from, NULL, NULL, 1);
}

static void *moronet8_session_thread(void *arg)
{
    moronet8_session *session = (moronet8_session *)arg;

    while (1)
    {
        pthread_mutex_lock(&session->mut);
        moronet8_session_dispatch_send(session);
        pthread_mutex_unlock(&session->mut);
        lws_service(session->context, 1);
    }
    return NULL;
}

static moronet8_session *moronet8_session_create(const char *host)
{
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);
    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));

    lws_context *context = lws_create_context(&info);
    if (!context)
    {
        lwsl_err("context init failed\n");
        return NULL;
    }

    ws_connect_options options;
    memset(&options, 0, sizeof(ws_connect_options));
    options.context = context;
    options.host = host;
    options.port = 2345;
    options.callback = &moronet8_session_callback;
    options.per_client_data_size = sizeof(moronet8_player);
    ws *socket = ws_connect(&options);
    if (!socket)
    {
        lws_context_destroy(context);
        lwsl_err("ws init failed\n");
        return NULL;
    }

    session.state = MORONET8_SESSION_CLOSED;
    session.context = context;
    session.socket = socket;

    pthread_create(&session.thread, NULL, moronet8_session_thread, (void *)&session);
    return &session;
}

moronet8_session *moronet8_session_host_impl(const char *host)
{
    if (!moronet8_session_create(host))
    {
        return NULL;
    }

    session.state = MORONET8_SESSION_CREATING;
    session.user_id = 1;
    session.host = MORONET8_TRUE;
    return &session;
}

moronet8_session *moronet8_session_join_impl(const char *host)
{
    if (!moronet8_session_create(host))
    {
        return NULL;
    }

    session.state = MORONET8_SESSION_JOINING;
    session.user_id = 2;
    session.host = MORONET8_FALSE;
    return &session;
}

void moronet8_session_delete_impl(moronet8_session *session)
{
    session->state = MORONET8_SESSION_CLOSED;
    if (session->context)
    {
        lws_context_destroy(session->context);
    }
    if (session->socket)
    {
        ws_delete(session->socket);
    }
}

moronet8_session_state moronet8_session_state_get_impl(moronet8_session *session)
{
    return session->state;
}

void moronet8_session_broadcast_impl(moronet8_session *session, const void *buf, size_t size)
{
    if (!session->player)
    {
        return;
    }

    packet_writer.offset = 0;
    moronet8_packet_write_u8(&packet_writer, MORONET8_PACKET_BROADCAST);
    memcpy(&packet_writer.buf[packet_writer.offset], buf, size);
    packet_writer.offset += size;
    moronet8_packet packet;
    memcpy(&packet.data, packet_writer.buf, packet_writer.offset);
    packet.size = packet_writer.offset;
    lws_ring_insert(session->player->wait_send_queue, &packet, 1);
}

size_t moronet8_session_receive_impl(moronet8_session *session, void *buf, size_t size)
{
    if (!session->player)
    {
        return 0;
    }

    const moronet8_packet *pmsg = lws_ring_get_element(session->player->receive_queue, 0);
    if (!pmsg)
    {
        return 0;
    }

    size_t len = pmsg->size;
    if (len > size)
    {
        len = size;
    }
    memcpy(buf, pmsg->data, len);
    lws_ring_consume(session->player->receive_queue, NULL, NULL, 1);
    return len;
}

void moronet8_session_poll_impl(moronet8_session *session)
{
    pthread_mutex_lock(&session->mut);
    if (session->player)
    {
        moronet8_session_move_queue(session, session->player->wait_receive_queue, session->player->receive_queue);
        moronet8_session_move_queue(session, session->player->wait_send_queue, session->player->send_queue);
    }
    pthread_mutex_unlock(&session->mut);
}
