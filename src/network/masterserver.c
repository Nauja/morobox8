#include "network/masterserver.h"

#include "libwebsockets.h"

#include <stdio.h>

typedef struct lws lws;
typedef struct lws_vhost lws_vhost;
typedef struct lws_context lws_context;
typedef struct lws_protocols lws_protocols;
typedef struct lws_context_creation_info lws_context_creation_info;
typedef struct lws_client_connect_info lws_client_connect_info;

static int
masterserver_callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                           void *in, size_t len)
{
    uint8_t buf[LWS_PRE + 256], *start = &buf[LWS_PRE], *p = start,
                                *end = &buf[sizeof(buf) - 1];
    int n;

    switch (reason)
    {
    case LWS_CALLBACK_HTTP:

        if (!lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI))
            /* not a GET */
            break;
        if (strcmp((const char *)in, "/form1"))
            /* not our form URL */
            break;

        /* we could add more headers here */

        if (lws_finalize_http_header(wsi, &p, end))
            return -1;

        n = lws_write(wsi, start, lws_ptr_diff(p, start),
                      LWS_WRITE_HTTP_HEADERS |
                          LWS_WRITE_H2_STREAM_END);
        if (n < 0)
            return -1;

        break;

    default:
        break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static lws_protocols masterserver_protocols[] = {
    {"http", masterserver_callback_http, 0, 0, 0, NULL, 0},
    {NULL, NULL, 0, 0, 0, NULL, 0} /* terminator */
};

lws_context *masterserver_create(lws_context *ctxt, int port)
{
    printf("create master server\n");
    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));
    info.port = port;
    info.protocols = masterserver_protocols;
    lws_vhost *server = lws_create_vhost(ctxt, &info);
    if (!server)
    {
        fprintf(stderr, "failed to create master server\n");
        return NULL;
    }

    printf("master server started on port %d\n", lws_get_vhost_listen_port(server));
    return ctxt;
}
