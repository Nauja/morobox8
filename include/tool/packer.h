#pragma once

#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if MOROBOX8_PACKER

    struct morobox8_cart_code_chunk;
    struct morobox8_cart_tileset_chunk;
    struct morobox8_cart;
    struct morobox8_packer;

    MOROBOX8_CREATE_H(morobox8_packer)

    /**
     * Set the function used to write cart data to output.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_set_writer(struct morobox8_packer *packer, size_t (*write)(struct morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size));

    /**
     * Set the output to write cart data to.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_set_output(struct morobox8_packer *packer, void *output);

#if MOROBOX8_FILESYSTEM
    /**
     * Create a packer that outputs to a file.
     */
    MOROBOX8_PUBLIC(struct morobox8_packer *)
    morobox8_file_packer(FILE *file);
#endif

    /**
     * Create a packer that outputs to memory.
     */
    MOROBOX8_PUBLIC(struct morobox8_packer *)
    morobox8_memory_packer(void *buf, size_t size);

    /**
     * Get the cart name.
     */
    MOROBOX8_PUBLIC(const char *)
    morobox8_packer_get_name(const struct morobox8_packer *packer);

    /**
     * Set the cart name.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_set_name(struct morobox8_packer *packer, const char *name);

    /**
     * Get the number of packed colors.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_get_num_colors(const struct morobox8_packer *packer);

#if MOROBOX8_FILESYSTEM
    /**
     * Pack the content of a directory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_packer_add_dir(struct morobox8_packer *packer, const char *path);
#endif

    /**
     * Add a file content to the packer.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_file(struct morobox8_packer *packer, const char *path);

    /**
     * Add a buffer content to the packer.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_memory(struct morobox8_packer *packer, const char *name, const void *buf, size_t size);

    /**
     * Add a color to the palette.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_packer_add_color(struct morobox8_packer *packer, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b);

    /**
     * Add code in the next available chunk.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_code(struct morobox8_packer *packer, const char *name, const void *buf, size_t size);

    /**
     * Add a PNG in the next available chunk.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_png(struct morobox8_packer *packer, const char *name, const void *buf, size_t size);

    /**
     * Add a chunk containing code.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_code_chunk(struct morobox8_packer *packer, const struct morobox8_cart_code_chunk *chunk);

    /**
     * Add a chunk containing a tileset.
     */
    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packer_add_tileset_chunk(struct morobox8_packer *packer, const struct morobox8_cart_tileset_chunk *chunk);

    /**
     * Select a code chunk.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_packer_select_code(struct morobox8_packer *packer, morobox8_u8 id);

    /**
     * Select a tileset chunk.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_packer_select_tileset(struct morobox8_packer *packer, morobox8_u8 id);

    /**
     * Select a font chunk.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_packer_select_font(struct morobox8_packer *packer, morobox8_u8 id);

    /**
     * Call after adding everything to the packer.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_packer_pack(struct morobox8_packer *packer);

#if MOROBOX8_FILESYSTEM
    /**
     * Pack a directory to file.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_pack_to_file(const char *src, const char *dst);
#endif

    /**
     * Pack a directory to memory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_pack_to_memory(const char *src, void *buf, size_t size);

    /**
     * Pack a directory to cart.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_pack_to_cart(const char *src, struct morobox8_cart *cart);

#endif

#ifdef __cplusplus
}
#endif
