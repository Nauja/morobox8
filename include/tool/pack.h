#pragma once

#include "morobox8_config.h"
#include "morobox8_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct morobox8_cart;
    struct morobox8_packer;

    MOROBOX8_CREATE_H(morobox8_packer)

#if MOROBOX8_FILESYSTEM
    /**
     * Add the content of a directory to the packer.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_add_dir(struct morobox8_packer *packer, const char *path);

    /**
     * Pack the content of a directory to a cart.
     */
    MOROBOX8_PUBLIC(struct morobox8_cart *)
    morobox8_pack(const char *path, struct morobox8_cart *cart);
#endif

    /**
     * Add a file content to the packer.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_add_file(struct morobox8_packer *packer, const char *name, void *buf, size_t size);

    /**
     * Pack everything into a cartridge.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_pack(struct morobox8_packer *packer, struct morobox8_cart *cart);

#ifdef __cplusplus
}
#endif
