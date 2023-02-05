#include "filesystem/filesystem.h"
#include "morobox8_defines.h"
#include "cart/cart.h"

/*
static morobox8_cart_chunk *morobox8_card_find_bank(morobox8_cart *cart, const char *name, morobox8_cart_chunk_type type, morobox8_u8 id)
{
    size_t i = 0;
    morobox8_cart_chunk *chunk = cart->chunks;
    while (i < cart->header.num_chunks)
    {
        if (name && strncmp(name, &chunk->name[0], MOROBOX8_FILENAME_SIZE) == 0)
        {
            return chunk;
        }

        if (chunk->type == type && chunk->id == id)
        {
            return chunk;
        }

        ++chunk;
        ++i;
    }

    return NULL;
}*/

MOROBOX8_PUBLIC(int)
morobox8_fs_read_cart_chunk(const char *name, size_t size, void *buf, size_t buf_size)
{
    return MOROBOX8_FALSE;
}
