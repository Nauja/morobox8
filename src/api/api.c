#include "api/api.h"

#if MOROBOX8_LUA_API
extern const struct morobox8_api_config morobox8_lua_api_config;
#endif

#if MOROBOX8_JS_API
extern const struct morobox8_api_config morobox8_js_api_config;
#endif

/* Configurations of the different APIs. */
const struct morobox8_api_config *morobox8_api_configs[] = {
#if MOROBOX8_LUA_API
    &morobox8_lua_api_config,
#endif
#if MOROBOX8_JS_API
    &morobox8_js_api_config,
#endif
    NULL};
