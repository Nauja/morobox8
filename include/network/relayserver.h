#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    struct ws;
    struct lws_context;

    struct ws *relayserver_create(struct lws_context *ctxt, int port);

#ifdef __cplusplus
}
#endif
