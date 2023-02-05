#pragma once

#include "morobox8_config.h"
#include "api/api_type.h"
#include "api/api_lang.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct morobox8_packer_chunk;
    struct morobox8_packer;
    struct morobox8;

    struct morobox8_api
    {
        /* Type of API. */
        enum morobox8_api_type type;
        /* State of the API. */
        void *state;
        /* Delete the API. */
        void (*free)(struct morobox8_api *api);
        /* Load code from buffer. */
        struct morobox8_api *(*load_string)(struct morobox8_api *api, const char *buf, size_t size);
        /* Tick the code. */
        struct morobox8_api *(*tick)(struct morobox8_api *api);
        /* Notify code is being unloaded. */
        void (*on_unload)(struct morobox8_api *api);
        /* Notify code has been loaded. */
        void (*on_load)(struct morobox8_api *api);
    };

    struct morobox8_api_config
    {
        /* Identifier. */
        enum morobox8_api_lang lang;
        /* Name of main file. */
        char *main;
        /* Size of main string. */
        size_t main_size;
        /* Extension. */
        char *ext;
        /* Size of extension string. */
        size_t ext_size;
    };

#ifdef __cplusplus
}
#endif
