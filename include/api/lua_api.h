#pragma once

#include "morobox8_config.h"
#include "api/api_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if MOROBOX8_LUA_API

    struct morobox8;
    struct morobox8_api;

    MOROBOX8_PUBLIC(struct morobox8_api *)
    morobox8_lua_api_init(struct morobox8_api *api, struct morobox8 *vm, enum morobox8_api_type type);

#endif

#ifdef __cplusplus
}
#endif
