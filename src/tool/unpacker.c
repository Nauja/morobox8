#include "tool/unpacker.h"

#if MOROBOX8_PACKER

#include "morobox8.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"
#include "api/lua_api.h"
#include "api/js_api.h"
#include "cart/cart.h"

#if MOROBOX8_FILESYSTEM
#include "fs.h"
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_unpacker morobox8_unpacker;

typedef struct morobox8_api_config morobox8_api_config;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8_cart_palette morobox8_cart_palette;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_any_chunk morobox8_cart_any_chunk;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_chunk_address morobox8_cart_chunk_address;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_max_size_cart morobox8_max_size_cart;
typedef struct morobox8_unpacker_chunks_iterator morobox8_unpacker_chunks_iterator;

struct morobox8_unpacker;
typedef struct morobox8_unpacker
{
    /* Function to read data from input. */
    void (*read)(struct morobox8_unpacker *unpacker, void *input, void *buf, size_t offset, size_t size);
    /* Input to read data from. */
    void *input;
} morobox8_unpacker;

struct morobox8_memory_unpacker
{
    /* Base unpacker. */
    morobox8_unpacker base;
    /* Size of input buffer. */
    size_t size;
};

typedef struct morobox8_unpacker_chunks_iterator_impl
{
    morobox8_unpacker_chunks_iterator base;
    morobox8_unpacker *unpacker;
    size_t chunk_id;
} morobox8_unpacker_chunks_iterator_impl;

MOROBOX8_CID_C(morobox8_unpacker)
MOROBOX8_CID_C(morobox8_unpacker_chunks_iterator_impl)

static struct morobox8_memory_unpacker *morobox8_memory_unpacker_create(void)
{
    struct morobox8_memory_unpacker *o = (struct morobox8_memory_unpacker *)morobox8_malloc(sizeof(struct morobox8_memory_unpacker));
    if (o)
    {
        memset(o, 0, sizeof(struct morobox8_memory_unpacker));
    }

    return o;
}

static inline void morobox8_unpacker_read(morobox8_unpacker *unpacker, void *buf, size_t offset, size_t size)
{
    if (unpacker->read)
    {
        unpacker->read(unpacker, unpacker->input, buf, offset, size);
    }
}

MOROBOX8_PUBLIC(void)
morobox8_unpacker_set_reader(morobox8_unpacker *unpacker, void (*read)(morobox8_unpacker *unpacker, void *input, void *buf, size_t offset, size_t size))
{
    unpacker->read = read;
}

MOROBOX8_PUBLIC(void)
morobox8_unpacker_set_input(morobox8_unpacker *unpacker, void *input)
{
    unpacker->input = input;
}

static void morobox8_unpacker_read_memory(morobox8_unpacker *unpacker, void *input, void *buf, size_t offset, size_t size)
{
    size_t input_size = ((struct morobox8_memory_unpacker *)unpacker)->size;
    if (offset >= input_size)
    {
        return;
    }

    memcpy(buf, (void *)&((morobox8_u8 *)input)[offset], min(size, input_size - offset));
}

MOROBOX8_PUBLIC(morobox8_unpacker *)
morobox8_memory_unpacker(void *buf, size_t size)
{
    struct morobox8_memory_unpacker *unpacker = morobox8_memory_unpacker_create();
    if (unpacker)
    {
        unpacker->base.read = &morobox8_unpacker_read_memory;
        unpacker->base.input = buf;
        unpacker->size = size;
    }

    return &unpacker->base;
}

MOROBOX8_PUBLIC(int)
morobox8_unpacker_read_header(morobox8_unpacker *unpacker, morobox8_cart_header *header)
{
    morobox8_unpacker_read(unpacker, header, 0, sizeof(morobox8_cart_header));
    return MOROBOX8_TRUE;
}

MOROBOX8_PUBLIC(int)
morobox8_unpacker_read_cart(morobox8_unpacker *unpacker, morobox8_cart *cart)
{
    morobox8_unpacker_read(unpacker, cart, 0, sizeof(morobox8_cart));
    return MOROBOX8_TRUE;
}

MOROBOX8_PUBLIC(int)
morobox8_unpacker_read_palette(morobox8_unpacker *unpacker, morobox8_cart_palette *palette)
{
    morobox8_unpacker_read(unpacker, palette, offsetof(morobox8_cart, data.palette), sizeof(morobox8_cart_palette));
    return MOROBOX8_TRUE;
}

MOROBOX8_PUBLIC(int)
morobox8_unpacker_read_chunk(morobox8_unpacker *unpacker, morobox8_u8 id, morobox8_cart_any_chunk *chunk)
{
    morobox8_cart_header header;
    if (!morobox8_unpacker_read_header(unpacker, &header) || id >= header.num_chunks)
    {
        return MOROBOX8_FALSE;
    }

    morobox8_cart_chunk *chunk_header = &header.chunks[id];
    memcpy(&chunk->code.base, chunk_header, sizeof(morobox8_cart_chunk));

    switch (chunk_header->type)
    {
    case MOROBOX8_CART_CHUNK_CODE:
        if (id == header.code_chunk)
        {
            chunk_header->address = offsetof(morobox8_cart, data.code);
        }
        morobox8_unpacker_read(unpacker, &chunk->code.code, chunk_header->address, sizeof(morobox8_cart_code));
        break;
    case MOROBOX8_CART_CHUNK_TILESET:
        if (id == header.tileset_chunk)
        {
            chunk_header->address = offsetof(morobox8_cart, data.tileset);
        }
        else if (id == header.font_chunk)
        {
            chunk_header->address = offsetof(morobox8_cart, data.font);
        }
        morobox8_unpacker_read(unpacker, &chunk->tileset.tileset, chunk_header->address, sizeof(morobox8_cart_tileset));
        break;
    default:
        return MOROBOX8_FALSE;
        break;
    }

    return MOROBOX8_TRUE;
}

MOROBOX8_PUBLIC(morobox8_unpacker_chunks_iterator *)
morobox8_unpacker_open_chunks(morobox8_unpacker *unpacker)
{
    morobox8_unpacker_chunks_iterator_impl *it = morobox8_unpacker_chunks_iterator_impl_create();
    if (it)
    {
        it->unpacker = unpacker;
        it->chunk_id = 0;
    }

    return &it->base;
}

MOROBOX8_PUBLIC(morobox8_unpacker_chunks_iterator *)
morobox8_unpacker_iter_chunks(morobox8_unpacker_chunks_iterator *it)
{
    if (!it)
    {
        return NULL;
    }

    morobox8_unpacker_chunks_iterator_impl *_it = (morobox8_unpacker_chunks_iterator_impl *)it;
    if (!morobox8_unpacker_read_chunk(_it->unpacker, _it->chunk_id, &_it->base.chunk))
    {
        return NULL;
    }

    _it->chunk_id++;
    return it;
}

MOROBOX8_PUBLIC(void)
morobox8_unpacker_close_chunks(morobox8_unpacker_chunks_iterator *it)
{
    morobox8_unpacker_chunks_iterator_impl_delete((morobox8_unpacker_chunks_iterator_impl *)it);
}

#if MOROBOX8_FILESYSTEM
static void morobox8_unpacker_read_file(morobox8_unpacker *unpacker, void *input, void *buf, size_t offset, size_t size)
{
    fseek((FILE *)input, offset, SEEK_SET);
    fread(buf, 1, size, (FILE *)input);
}

MOROBOX8_PUBLIC(morobox8_unpacker *)
morobox8_file_unpacker(FILE *file)
{
    morobox8_unpacker *unpacker = morobox8_unpacker_create();
    if (unpacker)
    {
        unpacker->read = &morobox8_unpacker_read_file;
        unpacker->input = (void *)file;
    }

    return unpacker;
}
#endif

MOROBOX8_PUBLIC(int)
morobox8_unpacker_unpack(morobox8_unpacker *unpacker)
{
    return 0;
}

MOROBOX8_PUBLIC(int)
morobox8_unpack_from_file(FILE *file, const char *dst)
{
    return 0;
}

MOROBOX8_PUBLIC(int)
morobox8_unpack_from_memory(void *buf, size_t size, const char *dst)
{
    morobox8_unpacker *unpacker = morobox8_memory_unpacker(buf, size);
    if (!unpacker)
    {
        return MOROBOX8_FALSE;
    }

    int result = morobox8_unpacker_unpack(unpacker);
    morobox8_unpacker_delete(unpacker);
    return result;
}

MOROBOX8_PUBLIC(int)
morobox8_pack_from_cart(morobox8_cart *cart, const char *dst)
{
    return morobox8_unpack_from_memory(cart, cart->header.size, dst);
}

#endif
