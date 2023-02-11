#pragma once

#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"
#include "cart/cart.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if MOROBOX8_PACKER

    struct morobox8_unpacker;

    /* Iterator over chunks. */
    struct morobox8_unpacker_chunks_iterator
    {
        struct morobox8_cart_any_chunk chunk;
    };

    MOROBOX8_CREATE_H(morobox8_unpacker)

    /**
     * Set the function used to read cart data from input.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_unpacker_set_reader(struct morobox8_unpacker *packer, void (*read)(struct morobox8_unpacker *unpacker, void *input, void *buf, size_t offset, size_t size));

    /**
     * Set the input to read cart data from.
     */
    MOROBOX8_PUBLIC(void)
    morobox8_unpacker_set_input(struct morobox8_unpacker *unpacker, void *input);

#if MOROBOX8_FILESYSTEM
    /**
     * Create a packer that reads from a file.
     */
    MOROBOX8_PUBLIC(struct morobox8_unpacker *)
    morobox8_file_unpacker(FILE *file);
#endif

    /**
     * Create a packer that reads from memory.
     */
    MOROBOX8_PUBLIC(struct morobox8_unpacker *)
    morobox8_memory_unpacker(void *buf, size_t size);

    /**
     * Read the cart header.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpacker_read_header(struct morobox8_unpacker *unpacker, struct morobox8_cart_header *header);

    /**
     * Read the cart.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpacker_read_cart(struct morobox8_unpacker *unpacker, struct morobox8_cart *cart);

    MOROBOX8_PUBLIC(int)
    morobox8_unpacker_read_palette(struct morobox8_unpacker *unpacker, struct morobox8_cart_palette *palette);

    /**
     * Read a chunk.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpacker_read_chunk(struct morobox8_unpacker *unpacker, morobox8_u8 id, struct morobox8_cart_any_chunk *chunk);

    MOROBOX8_PUBLIC(struct morobox8_unpacker_chunks_iterator *)
    morobox8_unpacker_open_chunks(struct morobox8_unpacker *unpacker);

    MOROBOX8_PUBLIC(struct morobox8_unpacker_chunks_iterator *)
    morobox8_unpacker_iter_chunks(struct morobox8_unpacker_chunks_iterator *it);

    MOROBOX8_PUBLIC(void)
    morobox8_unpacker_close_chunks(struct morobox8_unpacker_chunks_iterator *it);

    /**
     * Call to unpack everything to directory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpacker_unpack(struct morobox8_unpacker *unpacker);

#if MOROBOX8_FILESYSTEM
    /**
     * Unpack a cart file to directory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpack_from_file(FILE *file, const char *dst);
#endif

    /**
     * Unpack a cart in memory to directory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpack_from_memory(void *buf, size_t size, const char *dst);

    /**
     * Unpack a cart to directory.
     */
    MOROBOX8_PUBLIC(int)
    morobox8_unpack_from_cart(struct morobox8_cart *cart, const char *dst);

#endif

#ifdef __cplusplus
}
#endif
