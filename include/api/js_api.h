#pragma once

#include "moronet8_config.h"
#include "api/api_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if MORONET8_JS_API

    struct moronet8;
    struct moronet8_api;

    MORONET8_PUBLIC(struct moronet8_api *)
    moronet8_js_api_init(struct moronet8_api *api, struct moronet8 *vm, enum moronet8_api_type type);

#endif

#ifdef __cplusplus
}
#endif
