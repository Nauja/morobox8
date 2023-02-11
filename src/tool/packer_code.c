#include "tool/packer.h"

#if MOROBOX8_PACKER

#include "morobox8.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"
#include "api/lua_api.h"
#include "api/js_api.h"
#include "cart/cart.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_packer morobox8_packer;

typedef struct morobox8_api_config morobox8_api_config;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart morobox8_cart;

typedef enum morobox8_api_lang morobox8_api_lang;

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_code(morobox8_packer *packer, const char *name, const void *buf, size_t size)
{
    // Ensure the code can fit
    if (size > MOROBOX8_CART_CODE_SIZE)
    {
        morobox8_printf("Code size is too big\n");
        return MOROBOX8_CHUNK_INVALID;
    }

    // Name must contain the extension to determine language
    const char *pext = strrchr(name, '.');
    if (!pext)
    {
        morobox8_printf("Missing code extension %s\n", name);
        return MOROBOX8_CHUNK_INVALID;
    }

    size_t i = 0;
    const morobox8_api_config *config;
    while ((config = morobox8_api_configs[i]) != NULL)
    {
        // Match the extension with possible languages
        if (strncmp(pext, config->ext, config->ext_size) == 0)
        {
            morobox8_cart_code_chunk chunk;
            morobox8_cart_code_chunk_init(&chunk);
            snprintf(&chunk.base.name[0], MOROBOX8_CHUNK_NAME_SIZE, "%s", name);
            chunk.base.type = MOROBOX8_CART_CHUNK_CODE;
            chunk.code.lang = config->lang;
            memcpy(&chunk.code.text[0], buf, min(MOROBOX8_CART_CODE_SIZE, size));
            return morobox8_packer_add_code_chunk(packer, &chunk);
        }

        ++i;
    }

    morobox8_printf("Unknown code extension %s\n", name);
    return MOROBOX8_CHUNK_INVALID;
}

#endif
