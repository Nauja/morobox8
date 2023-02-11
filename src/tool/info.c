#include "tool/info.h"

#if MOROBOX8_INFO

#include "morobox8.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"
#include "cart/cart.h"
#include "tool/packer.h"
#include "tool/unpacker.h"

#if MOROBOX8_FILESYSTEM
#include "fs.h"
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_palette morobox8_cart_palette;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_unpacker_chunks_iterator morobox8_unpacker_chunks_iterator;
typedef struct morobox8_unpacker morobox8_unpacker;
typedef struct morobox8_cart_any_chunk morobox8_cart_any_chunk;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef enum morobox8_info_level morobox8_info_level;

#define MOROBOX8_INFO_ADD(fmt, ...) \
    offset += snprintf(&buf[offset], size - offset, fmt, __VA_ARGS__);
#define MOROBOX8_INFO_FIELD(name, fmt, ...) \
    MOROBOX8_INFO_ADD("- " name ": " fmt "\n", __VA_ARGS__)
#define MOROBOX8_INFO_CHUNK_TYPE(type) \
    MOROBOX8_INFO_ADD("%s", type == MOROBOX8_CART_CHUNK_CODE ? "code" : "tileset")
#define MOROBOX8_INFO_CHUNK_TYPE2(type) \
    MOROBOX8_INFO_ADD("%s", type == MOROBOX8_CART_CHUNK_CODE ? "Code" : "Tileset")
#define MOROBOX8_INFO_SELECTED_CHUNK(name, id)   \
    if (id == MOROBOX8_CHUNK_INVALID)            \
        MOROBOX8_INFO_FIELD(name, "%s", "empty") \
    else                                         \
        MOROBOX8_INFO_FIELD(name, "%d", id)

static int morobox8_info_chunk_common_header(morobox8_cart_chunk *chunk, char *buf, size_t size)
{
    int offset = 0;
    MOROBOX8_INFO_CHUNK_TYPE2(chunk->type)
    MOROBOX8_INFO_ADD(" Chunk:%s\n", "")
    MOROBOX8_INFO_FIELD("Id", "%d", chunk->id)
    MOROBOX8_INFO_FIELD("Name", "%s", chunk->name)
    return offset;
}

static int morobox8_info_code_chunk(morobox8_cart_code_chunk *chunk, char *buf, size_t size, morobox8_info_level level)
{
    int offset = 0;
    size_t len = strlen(chunk->code.text);
    MOROBOX8_INFO_FIELD("Size", "%d bytes (%d%% of total space)", len, MOROBOX8_PERCENT_INT(len, MOROBOX8_CART_CODE_SIZE))
    MOROBOX8_INFO_FIELD("Address", "0x%0x", chunk->base.address)
    if (level == MOROBOX8_INFO_EXTRA_VERBOSE)
    {
        MOROBOX8_INFO_ADD("\n%s", "")
        MOROBOX8_INFO_ADD("%s\n", chunk->code.text);
    }
    return offset;
}

static int morobox8_info_tileset_chunk(morobox8_cart_tileset_chunk *chunk, char *buf, size_t size, morobox8_info_level level)
{
    int offset = 0;
    MOROBOX8_INFO_FIELD("Address", "0x%0x", chunk->base.address)
    return offset;
}

static int morobox8_info_chunk(morobox8_cart_any_chunk *chunk, char *buf, size_t size, morobox8_info_level level)
{
    int offset = morobox8_info_chunk_common_header(&chunk->code.base, buf, size);
    switch (chunk->code.base.type)
    {
    case MOROBOX8_CART_CHUNK_CODE:
        return offset + morobox8_info_code_chunk(&chunk->code, &buf[offset], size - offset, level);
    case MOROBOX8_CART_CHUNK_TILESET:
        return offset + morobox8_info_tileset_chunk(&chunk->tileset, &buf[offset], size - offset, level);
    default:
        assert(MOROBOX8_FALSE);
        return 0;
    }
}

static int morobox8_info_palette(morobox8_cart_palette *data, size_t num_colors, char *buf, size_t size)
{
    int offset = 0;
    MOROBOX8_INFO_ADD("\nPalette:%s\n", "")
    morobox8_cart_color *color = &data->colors[0];
    for (size_t i = 0; i < num_colors; ++i)
    {
        MOROBOX8_INFO_ADD("- %d: 0x%0.6X\n", i, (color->r << 16) + (color->g << 8) + color->b);
        ++color;
    }
    return offset;
}

static int morobox8_info_unpacker(morobox8_unpacker *unpacker, char *buf, size_t size, size_t *out_size, morobox8_info_level level)
{
    morobox8_cart_header header;
    morobox8_unpacker_read_header(unpacker, &header);
    int offset = 0;
    MOROBOX8_INFO_ADD("Header:%s\n", "")
    MOROBOX8_INFO_FIELD("Magic Code", "%s", header.magic_code)
    MOROBOX8_INFO_FIELD("Name", "%s", header.name)
    MOROBOX8_INFO_FIELD("Size", "%d bytes (%d%% of total space)", header.size, MOROBOX8_PERCENT_INT(header.size, MOROBOX8_CART_MAX_SIZE))
    MOROBOX8_INFO_FIELD("Chunks", "%d", header.num_chunks)
    MOROBOX8_INFO_SELECTED_CHUNK("Code Chunk", header.code_chunk)
    MOROBOX8_INFO_SELECTED_CHUNK("Tileset Chunk", header.tileset_chunk)
    MOROBOX8_INFO_SELECTED_CHUNK("Font Chunk", header.font_chunk)
    MOROBOX8_INFO_FIELD("Colors", "%d", header.num_colors)

    if (level == MOROBOX8_EXTRA_VERBOSE)
    {
        morobox8_cart_palette palette;
        morobox8_unpacker_read_palette(unpacker, &palette);
        offset += morobox8_info_palette(&palette, header.num_colors, &buf[offset], size - offset);
    }

    morobox8_unpacker_chunks_iterator *it = morobox8_unpacker_open_chunks(unpacker);
    while ((it = morobox8_unpacker_iter_chunks(it)) != NULL)
    {
        MOROBOX8_INFO_ADD("\n%s", "")
        offset += morobox8_info_chunk(&it->chunk, &buf[offset], size - offset, level);
    }
    morobox8_unpacker_close_chunks(it);
#undef MOROBOX8_INFO_CHUNK_TYPE
    *out_size = (size_t)offset;
    return MOROBOX8_TRUE;
}

#undef MOROBOX8_INFO_ADD

#if MOROBOX8_FILESYSTEM
MOROBOX8_PUBLIC(int)
morobox8_info_file(FILE *file, char *buf, size_t size, size_t *out_size, morobox8_info_level level)
{
    morobox8_unpacker *unpacker = morobox8_file_unpacker(file);
    if (!unpacker)
    {
        return MOROBOX8_FALSE;
    }

    int result = morobox8_info_unpacker(unpacker, buf, size, out_size, level);
    morobox8_unpacker_delete(unpacker);
    return result;
}
#endif

MOROBOX8_PUBLIC(int)
morobox8_info_cart(morobox8_cart *cart, char *buf, size_t size, size_t *out_size, morobox8_info_level level)
{
    morobox8_unpacker *unpacker = morobox8_memory_unpacker(cart, cart->header.size);
    if (!unpacker)
    {
        return MOROBOX8_FALSE;
    }

    int result = morobox8_info_unpacker(unpacker, buf, size, out_size, level);
    morobox8_unpacker_delete(unpacker);
    return result;
}

#endif
