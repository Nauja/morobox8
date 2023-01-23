#include "moronet8.h"
#include "libwebsockets.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct moronet8_socket moronet8_socket;
typedef struct lws lws;
typedef struct lws_context_creation_info lws_context_creation_info;
typedef struct lws_client_connect_info lws_client_connect_info;

moronet8_socket *moronet8_socket_listen_impl(int port)
{
    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));

    printf("socket created\n");
    return (moronet8_socket *)lws_create_context(&info);
}

moronet8_socket *moronet8_socket_connect_impl(const char *host)
{
    lws_client_connect_info info;
    memset(&info, 0, sizeof(lws_client_connect_info));
    printf("%s\n", host);
    info.address = "localhost";
    info.port = 1234;

    return (moronet8_socket *)lws_client_connect_via_info(&info);
}

void moronet8_socket_close_impl(moronet8_socket *socket)
{
    lws *s = (lws *)socket;
    printf("%p", s);
}
