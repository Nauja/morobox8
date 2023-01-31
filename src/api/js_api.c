#include "morobox8.h"

#if MOROBOX8_JS_API

#include "api/js_api.h"

typedef enum morobox8_api_type morobox8_api_type;
typedef struct morobox8_api morobox8_api;
typedef struct morobox8 morobox8;

static void morobox8_js_api_free(morobox8_api *api)
{
}

MOROBOX8_PUBLIC(morobox8_api *)
morobox8_js_api_init(morobox8_api *api, morobox8 *vm, morobox8_api_type type)
{
    api->free = &morobox8_js_api_free;
    return api;
}

#endif
