#include "moronet8.h"

#if MORONET8_JS_API

#include "api/js_api.h"

typedef enum moronet8_api_type moronet8_api_type;
typedef struct moronet8_api moronet8_api;
typedef struct moronet8 moronet8;

static void moronet8_js_api_delete(moronet8_api *api)
{
}

MORONET8_PUBLIC(moronet8_api *)
moronet8_js_api_init(moronet8_api *api, moronet8 *vm, moronet8_api_type type)
{
    api->delete = &moronet8_js_api_delete;
    return api;
}

#endif
