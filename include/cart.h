#pragma once

#include "morobox8_config.h"
#include "morobox8_types.h"
#include "morobox8_limits.h"
#include "morobox8_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MOROBOX8_CART_MAGIC_CODE "MB8\n"
#define MOROBOX8_CART_CODE_SIZE 0x3FFF

    enum morobox8_lang
    {
#if MOROBOX8_LUA_API
        MOROBOX8_LANG_LUA = 0x1,
#endif
#if MOROBOX8_JS_API
        MOROBOX8_LANG_JS = 0x2,
#endif
    };

    enum morobox8_cart_chunk_type
    {
        MOROBOX8_CART_CHUNK_TILESET,
        MOROBOX8_CART_CHUNK_CODE,
    };

    struct morobox8_cart_chunk;

    /* A chunk of arbitrary data in the cart. */
    struct morobox8_cart_chunk
    {
        /* Unique id of the chunk. */
        morobox8_u8 id;
        /* Type of chunk. */
        morobox8_u8 type;
        /* Pointer to next chunk. */
        struct morobox8_cart_chunk *next;
    };

    struct morobox8_cart_color
    {
        morobox8_u8 r;
        morobox8_u8 g;
        morobox8_u8 b;
        morobox8_u8 t;
    };

    /* A single sprite on the tileset. */
    struct morobox8_cart_sprite
    {
        /* Pointer to pixels. */
        morobox8_u8 pixels[MOROBOX8_PIXELS_PER_SPRITE];
        /* Color palette. */
        morobox8_u8 palette[MOROBOX8_COLORS_PER_SPRITE];
        /* Number of colors in the sprite. */
        morobox8_u8 num_colors;
    };

    /* A single tileset. */
    struct morobox8_cart_tileset
    {
        /* List of sprites. */
        struct morobox8_cart_sprite sprites[MOROBOX8_TILESET_SIZE];
    };

    /* Chunk containing a tileset. */
    struct morobox8_cart_tileset_chunk
    {
        /* Chunk data. */
        struct morobox8_cart_chunk base;
        /* Tileset data. */
        struct morobox8_cart_tileset tileset;
    };

    /* Code with lang indication. */
    struct morobox8_cart_code
    {
        /* Lang of the code. */
        morobox8_u8 lang;
        /* Code. */
        char text[MOROBOX8_CART_CODE_SIZE];
    };

    /* Chunk containing some code. */
    struct morobox8_cart_code_chunk
    {
        /* Chunk data. */
        struct morobox8_cart_chunk base;
        /* Code data. */
        struct morobox8_cart_code code;
    };

    /* Cart header. */
    struct morobox8_cart_header
    {
        /* Magic code. */
        char magic_code[4];
        /* Number of chunks are data section. */
        morobox8_u8 num_chunks;
    };

    /* Cart data. */
    struct morobox8_cart_data
    {
        /* Color palette. */
        struct morobox8_cart_color palette[MOROBOX8_PALETTE_SIZE];
        /* Number of colors in the palette. */
        morobox8_u8 num_colors;
        /* Tileset section. */
        struct morobox8_cart_tileset tileset;
        /* Font section. */
        struct morobox8_cart_tileset font;
        /* Code section. */
        struct morobox8_cart_code code;
    };

    /* Cart data. */
    struct morobox8_cart
    {
        /* Cart header. */
        struct morobox8_cart_header header;
        /* Cart data. */
        struct morobox8_cart_data data;
        /* Cart chunks. */
        struct morobox8_cart_chunk *chunks;
    };

    MOROBOX8_CREATE_H(morobox8_cart_code_chunk)
    MOROBOX8_CREATE_H(morobox8_cart_tileset_chunk)
    MOROBOX8_CREATE_H(morobox8_cart_header)
    MOROBOX8_CREATE_H(morobox8_cart_data)
    MOROBOX8_CREATE_H(morobox8_cart)

    MOROBOX8_PUBLIC(size_t)
    morobox8_cart_sizeof(void);

    MOROBOX8_PUBLIC(size_t)
    morobox8_cart_data_as_buffer(const struct morobox8_cart_data *data, void *buffer, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_data_from_buffer(struct morobox8_cart_data *data, const void *buffer, size_t size);

    MOROBOX8_PUBLIC(struct morobox8_cart_header *)
    morobox8_cart_get_header(struct morobox8_cart *cart);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_set_header(struct morobox8_cart *cart, const struct morobox8_cart_header *header);

    MOROBOX8_PUBLIC(struct morobox8_cart_data *)
    morobox8_cart_get_data(struct morobox8_cart *cart);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_set_data(struct morobox8_cart *cart, const struct morobox8_cart_data *data);

    MOROBOX8_PUBLIC(size_t)
    morobox8_cart_as_buffer(const struct morobox8_cart *cart, void *buffer, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_from_buffer(struct morobox8_cart *cart, const void *buffer, size_t size);

    MOROBOX8_PUBLIC(enum morobox8_lang)
    morobox8_cart_get_lang(struct morobox8_cart *cart);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_set_lang(struct morobox8_cart *cart, enum morobox8_lang lang);

#if MOROBOX8_FILESYSTEM
    MOROBOX8_PUBLIC(void)
    morobox8_cart_load_file(struct morobox8_cart *cart, const char *path);

    MOROBOX8_PUBLIC(size_t)
    morobox8_cart_dump_file(struct morobox8_cart *cart, const char *path);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_load_dir(struct morobox8_cart *cart, const char *path);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_dump_dir(struct morobox8_cart *cart, const char *path);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_load(struct morobox8_cart *cart, const void *buf, size_t size);

    MOROBOX8_PUBLIC(size_t)
    morobox8_cart_dump(struct morobox8_cart *cart, void *buf, size_t size);
#endif

    MOROBOX8_PUBLIC(void)
    morobox8_cart_select_font(struct morobox8_cart *cart, morobox8_u8 id);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_select_tileset(struct morobox8_cart *cart, morobox8_u8 id);

    MOROBOX8_PUBLIC(void)
    morobox8_cart_select_code(struct morobox8_cart *cart, morobox8_u8 id);

#ifdef __cplusplus
}
#endif
