#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    struct lws_context;

    struct lws_context *masterserver_create(struct lws_context *ctxt, int port);

#ifdef __cplusplus
}
#endif
