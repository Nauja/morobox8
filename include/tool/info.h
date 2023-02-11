#pragma once

#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if MOROBOX8_PACKER

    struct morobox8_cart;

    enum morobox8_info_level
    {
        MOROBOX8_INFO_SIMPLE = 0,
        MOROBOX8_INFO_VERBOSE,
        MOROBOX8_INFO_EXTRA_VERBOSE
    };

#if MOROBOX8_FILESYSTEM
    /**
     * Get information about a cart from directory or file.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_info_file(FILE *file, char *buf, size_t size, size_t *out_size, enum morobox8_info_level level);
#endif

    /**
     * Print information about a cart.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_info_cart(struct morobox8_cart *cart, char *buf, size_t size, size_t *out_size, enum morobox8_info_level level);

#endif

#ifdef __cplusplus
}
#endif
