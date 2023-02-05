#pragma once

#include "morobox8_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum morobox8_api_lang
    {
#if MOROBOX8_LUA_API
        MOROBOX8_API_LANG_LUA = 0x1,
#endif
#if MOROBOX8_JS_API
        MOROBOX8_API_LANG_JS = 0x2,
#endif
    };

#ifdef __cplusplus
}
#endif
