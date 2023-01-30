#pragma once

#include "moronet8_config.h"
#include "moronet8_types.h"
#include "moronet8_limits.h"
#include "moronet8_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MORONET8_CART_MAGIC_CODE "MB8\n"
#define MORONET8_CART_CODE_SIZE 0x3FFF

    enum moronet8_lang
    {
#if MORONET8_LUA_API
        MORONET8_LANG_LUA = 0x1,
#endif
#if MORONET8_JS_API
        MORONET8_LANG_JS = 0x2,
#endif
    };

    enum moronet8_cart_chunk_type
    {
        MORONET8_CART_CHUNK_TILESET,
        MORONET8_CART_CHUNK_CODE,
    };

    struct moronet8_cart_chunk;

    /* A chunk of arbitrary data in the cart. */
    struct moronet8_cart_chunk
    {
        /* Unique id of the chunk. */
        moronet8_u8 id;
        /* Type of chunk. */
        moronet8_u8 type;
        /* Pointer to next chunk. */
        struct moronet8_cart_chunk *next;
    };

    struct moronet8_cart_color
    {
        moronet8_u8 r;
        moronet8_u8 g;
        moronet8_u8 b;
        moronet8_u8 t;
    };

    /* A single sprite on the tileset. */
    struct moronet8_cart_sprite
    {
        /* Pointer to pixels. */
        moronet8_u8 pixels[MORONET8_PIXELS_PER_SPRITE];
        /* Color palette. */
        moronet8_u8 palette[MORONET8_COLORS_PER_SPRITE];
        /* Number of colors in the sprite. */
        moronet8_u8 num_colors;
    };

    /* A single tileset. */
    struct moronet8_cart_tileset
    {
        /* List of sprites. */
        struct moronet8_cart_sprite sprites[MORONET8_TILESET_SIZE];
    };

    /* Chunk containing a tileset. */
    struct moronet8_cart_tileset_chunk
    {
        /* Chunk data. */
        struct moronet8_cart_chunk base;
        /* Tileset data. */
        struct moronet8_cart_tileset tileset;
    };

    /* Code with lang indication. */
    struct moronet8_cart_code
    {
        /* Lang of the code. */
        moronet8_u8 lang;
        /* Code. */
        char text[MORONET8_CART_CODE_SIZE];
    };

    /* Chunk containing some code. */
    struct moronet8_cart_code_chunk
    {
        /* Chunk data. */
        struct moronet8_cart_chunk base;
        /* Code data. */
        struct moronet8_cart_code code;
    };

    /* Cart header. */
    struct moronet8_cart_header
    {
        /* Magic code. */
        char magic_code[4];
        /* Number of chunks are data section. */
        moronet8_u8 num_chunks;
    };

    /* Cart data. */
    struct moronet8_cart_data
    {
        /* Color palette. */
        struct moronet8_cart_color palette[MORONET8_PALETTE_SIZE];
        /* Number of colors in the palette. */
        moronet8_u8 num_colors;
        /* Tileset section. */
        struct moronet8_cart_tileset tileset;
        /* Font section. */
        struct moronet8_cart_tileset font;
        /* Code section. */
        struct moronet8_cart_code code;
    };

    /* Cart data. */
    struct moronet8_cart
    {
        /* Cart header. */
        struct moronet8_cart_header header;
        /* Cart data. */
        struct moronet8_cart_data data;
        /* Cart chunks. */
        struct moronet8_cart_chunk *chunks;
    };

    MORONET8_CREATE_H(moronet8_cart_code_chunk)
    MORONET8_CREATE_H(moronet8_cart_tileset_chunk)
    MORONET8_CREATE_H(moronet8_cart_header)
    MORONET8_CREATE_H(moronet8_cart_data)
    MORONET8_CREATE_H(moronet8_cart)

    MORONET8_PUBLIC(size_t)
    moronet8_cart_sizeof(void);

    MORONET8_PUBLIC(size_t)
    moronet8_cart_data_as_buffer(const struct moronet8_cart_data *data, void *buffer, size_t size);

    MORONET8_PUBLIC(void)
    moronet8_cart_data_from_buffer(struct moronet8_cart_data *data, const void *buffer, size_t size);

    MORONET8_PUBLIC(struct moronet8_cart_header *)
    moronet8_cart_get_header(struct moronet8_cart *cart);

    MORONET8_PUBLIC(void)
    moronet8_cart_set_header(struct moronet8_cart *cart, const struct moronet8_cart_header *header);

    MORONET8_PUBLIC(struct moronet8_cart_data *)
    moronet8_cart_get_data(struct moronet8_cart *cart);

    MORONET8_PUBLIC(void)
    moronet8_cart_set_data(struct moronet8_cart *cart, const struct moronet8_cart_data *data);

    MORONET8_PUBLIC(size_t)
    moronet8_cart_as_buffer(const struct moronet8_cart *cart, void *buffer, size_t size);

    MORONET8_PUBLIC(void)
    moronet8_cart_from_buffer(struct moronet8_cart *cart, const void *buffer, size_t size);

    MORONET8_PUBLIC(enum moronet8_lang)
    moronet8_cart_get_lang(struct moronet8_cart *cart);

    MORONET8_PUBLIC(void)
    moronet8_cart_set_lang(struct moronet8_cart *cart, enum moronet8_lang lang);

#if MORONET8_FILESYSTEM
    MORONET8_PUBLIC(void)
    moronet8_cart_load_file(struct moronet8_cart *cart, const char *path);

    MORONET8_PUBLIC(size_t)
    moronet8_cart_dump_file(struct moronet8_cart *cart, const char *path);

    MORONET8_PUBLIC(void)
    moronet8_cart_load_dir(struct moronet8_cart *cart, const char *path);

    MORONET8_PUBLIC(void)
    moronet8_cart_dump_dir(struct moronet8_cart *cart, const char *path);

    MORONET8_PUBLIC(void)
    moronet8_cart_load(struct moronet8_cart *cart, const void *buf, size_t size);

    MORONET8_PUBLIC(size_t)
    moronet8_cart_dump(struct moronet8_cart *cart, void *buf, size_t size);
#endif

    MORONET8_PUBLIC(void)
    moronet8_cart_select_font(struct moronet8_cart *cart, moronet8_u8 id);

    MORONET8_PUBLIC(void)
    moronet8_cart_select_tileset(struct moronet8_cart *cart, moronet8_u8 id);

    MORONET8_PUBLIC(void)
    moronet8_cart_select_code(struct moronet8_cart *cart, moronet8_u8 id);

#ifdef __cplusplus
}
#endif
