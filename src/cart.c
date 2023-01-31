#include "cart.h"
#include "morobox8.h"
#include "morobox8_types.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if MOROBOX8_FILESYSTEM
#include <png.h>
#include <fs.h>

typedef struct fs_directory_iterator fs_directory_iterator;
#endif

typedef enum morobox8_lang morobox8_lang;
typedef enum morobox8_cart_chunk_type morobox8_cart_chunk_type;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_data morobox8_cart_data;

MOROBOX8_CID_C(morobox8_cart_code_chunk)
MOROBOX8_CID_C(morobox8_cart_tileset_chunk)
MOROBOX8_CID_C(morobox8_cart_header)
MOROBOX8_CID_C(morobox8_cart_data)
MOROBOX8_CID_C(morobox8_cart)

#define min(a, b) (a < b ? a : b)

MOROBOX8_PUBLIC(size_t)
morobox8_cart_sizeof(void)
{
    return sizeof(struct morobox8_cart);
}

MOROBOX8_PUBLIC(size_t)
morobox8_cart_data_as_buffer(const morobox8_cart_data *data, void *buffer, size_t size)
{
    memcpy(buffer, data, min(sizeof(morobox8_cart_data), size));
    return sizeof(morobox8_cart_data);
}

MOROBOX8_PUBLIC(void)
morobox8_cart_data_from_buffer(morobox8_cart_data *data, const void *buffer, size_t size)
{
    memcpy(data, buffer, min(sizeof(morobox8_cart_data), size));
}

MOROBOX8_PUBLIC(morobox8_cart_header *)
morobox8_cart_get_header(morobox8_cart *cart)
{
    return &cart->header;
}

MOROBOX8_PUBLIC(void)
morobox8_cart_set_header(morobox8_cart *cart, const morobox8_cart_header *header)
{
    memcpy(&cart->header, header, sizeof(morobox8_cart_header));
}

MOROBOX8_PUBLIC(morobox8_cart_data *)
morobox8_cart_get_data(morobox8_cart *cart)
{
    return &cart->data;
}

MOROBOX8_PUBLIC(void)
morobox8_cart_set_data(morobox8_cart *cart, const morobox8_cart_data *data)
{
    memcpy(&cart->data, data, sizeof(morobox8_cart_data));
}

MOROBOX8_PUBLIC(size_t)
morobox8_cart_as_buffer(const morobox8_cart *cart, void *buffer, size_t size)
{
    memcpy(buffer, cart, min(sizeof(morobox8_cart), size));
    return sizeof(morobox8_cart);
}

MOROBOX8_PUBLIC(void)
morobox8_cart_from_buffer(morobox8_cart *cart, const void *buffer, size_t size)
{
    memcpy(cart, buffer, min(sizeof(morobox8_cart), size));
}

MOROBOX8_PUBLIC(morobox8_lang)
morobox8_cart_get_lang(morobox8_cart *cart)
{
    return cart->data.code.lang;
}

MOROBOX8_PUBLIC(void)
morobox8_cart_set_lang(morobox8_cart *cart, morobox8_lang lang)
{
    cart->data.code.lang = lang;
}

#if MOROBOX8_FILESYSTEM
static struct
{
    const char *ext;
    morobox8_lang lang;
} morobox8_scripts[] = {
#if MOROBOX8_LUA_API
    {".lua", MOROBOX8_LANG_LUA},
#endif
#if MOROBOX8_JS_API
    {".js", MOROBOX8_LANG_JS},
#endif
    {NULL, MOROBOX8_LANG_LUA},
};

MOROBOX8_PUBLIC(void)
morobox8_cart_load_file(morobox8_cart *cart, const char *path)
{
    size_t size = 0;
    void *buf = fs_read_file(path, &size);
    if (!buf)
    {
        return;
    }

    morobox8_cart_load(cart, buf, size);

    MOROBOX8_FREE(buf);
}

MOROBOX8_PUBLIC(size_t)
morobox8_cart_dump_file(morobox8_cart *cart, const char *path)
{
    size_t cart_size = sizeof(morobox8_cart);
    if (fs_write_file(path, (const void *)cart, cart_size))
    {
        return cart_size;
    }

    return 0;
}

static morobox8_u8 morobox8_palette_add_color(morobox8_cart *cart, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    morobox8_cart_color *color = &cart->data.palette[0];
    for (morobox8_u8 i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        if (i >= cart->data.num_colors)
        {
            color->r = r;
            color->g = g;
            color->b = b;
            cart->data.num_colors++;
            morobox8_printf("index %u color (%u, %u, %u)\n", i, r, g, b);
            return i;
        }

        if (color->r != r || color->g != g || color->b != b)
        {
            ++color;
            continue;
        }

        return i;
    }

    return 0;
}

static morobox8_u8 morobox8_sprite_palette_add_color(morobox8_cart *cart, morobox8_cart_sprite *sprite, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    morobox8_u8 id = morobox8_palette_add_color(cart, r, g, b);
    for (morobox8_u8 i = 0; i < MOROBOX8_COLORS_PER_SPRITE; ++i)
    {
        if (i >= sprite->num_colors)
        {
            sprite->palette[i] = id;
            sprite->num_colors++;
            return id;
        }

        if (sprite->palette[i] == id)
        {
            return id;
        }
    }

    return 0;
}

static morobox8_cart_tileset *morobox8_cart_load_png(morobox8_cart *cart, morobox8_cart_tileset *tileset, const char *path, morobox8_u8 id)
{
    char name[256];
    snprintf(name, 256, "bank%u.png", id);

    char buf[256];
    fs_join_path(buf, 256, path, name);
    if (!fs_exist(buf))
    {
        return NULL;
    }

    morobox8_printf("load %s\n", name);

    FILE *fp = fopen(buf, "rb");
    if (!fp)
    {
        return NULL;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        return NULL;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        return NULL;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        return NULL;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    morobox8_u8 *data = malloc(sizeof(morobox8_u32) * width * height);
    png_bytep *rows = (png_bytep *)malloc(sizeof(png_bytep) * height);

    for (int i = 0; i < height; i++)
        rows[i] = data + width * i * sizeof(morobox8_u32);

    png_read_image(png, rows);

    free(rows);

    morobox8_u8 tiles_w = (morobox8_u8)(width / 8);
    if (tiles_w > MOROBOX8_TILESET_WIDTH)
    {
        tiles_w = MOROBOX8_TILESET_WIDTH;
    }

    morobox8_u8 tiles_h = (morobox8_u8)(height / 8);
    if (tiles_h > MOROBOX8_TILESET_HEIGHT)
    {
        tiles_h = MOROBOX8_TILESET_HEIGHT;
    }

    morobox8_printf("tile contains (%u, %u) sprites\n", tiles_w, tiles_h);

    morobox8_u8 *pdata;
    morobox8_cart_sprite *sprite = &tileset->sprites[0];
    morobox8_u8 *pixels;
    for (int tile_j = 0; tile_j < tiles_h; ++tile_j)
    {
        for (int tile_i = 0; tile_i < tiles_w; ++tile_i)
        {
            sprite->num_colors = 0;
            pixels = &sprite->pixels[0];
            pdata = &data[((tile_i * 8) + ((tile_j * 8) * width)) * 4];
            for (morobox8_u8 j = 0; j < MOROBOX8_SPRITE_HEIGHT; ++j)
            {
                for (morobox8_u8 i = 0; i < MOROBOX8_SPRITE_WIDTH; ++i)
                {
                    *pixels = morobox8_sprite_palette_add_color(
                        cart,
                        sprite,
                        pdata[0],
                        pdata[1],
                        pdata[2]);
                    pdata += 4;
                    ++pixels;
                }
                pdata += (width * 4) - MOROBOX8_SPRITE_WIDTH * 4;
            }
            ++sprite;
        }
    }
    morobox8_printf("%u indexed colors\n", cart->data.num_colors);

    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    free(data);
    return tileset;
}

static void morobox8_cart_load_tileset(morobox8_cart *cart, const char *path, morobox8_u8 id)
{
    morobox8_cart_tileset_chunk *chunk = morobox8_cart_tileset_chunk_create();
    if (!chunk)
    {
        return;
    }

    if (!morobox8_cart_load_png(cart, &chunk->tileset, path, id))
    {
        morobox8_cart_tileset_chunk_delete(chunk);
        return;
    }

    chunk->base.type = MOROBOX8_CART_CHUNK_TILESET;
    chunk->base.id = id;
    chunk->base.next = cart->chunks;
    cart->chunks = &chunk->base;
}

static morobox8_cart_code *morobox8_cart_load_script(morobox8_cart_code *code, const char *path, morobox8_u8 id, const char *ext)
{
    char name[256];
    snprintf(name, 256, "bank%u%s", id, ext);

    char buf[256];
    fs_join_path(buf, 256, path, name);
    if (!fs_exist(buf))
    {
        return NULL;
    }

    morobox8_printf("load %s\n", name);

    fs_read_file_buffer(buf, (void *)&code->text[0], MOROBOX8_CART_CODE_SIZE);
    return code;
}

static void morobox8_cart_load_code(morobox8_cart *cart, const char *path, morobox8_u8 id, const char *ext, morobox8_lang lang)
{
    morobox8_cart_code_chunk *chunk = morobox8_cart_code_chunk_create();
    if (!chunk)
    {
        return;
    }

    if (!morobox8_cart_load_script(&chunk->code, path, id, ext))
    {
        morobox8_cart_code_chunk_delete(chunk);
        return;
    }

    chunk->base.type = MOROBOX8_CART_CHUNK_CODE;
    chunk->base.id = id;
    chunk->code.lang = lang;
    chunk->base.next = cart->chunks;
    cart->chunks = &chunk->base;
}

MOROBOX8_PUBLIC(void)
morobox8_cart_load_dir(morobox8_cart *cart, const char *path)
{
    morobox8_printf("load dir %s\n", path);
    size_t script_index = 0;
    for (size_t i = 0; i < 256; ++i)
    {
        morobox8_cart_load_tileset(cart, path, (morobox8_u8)i);

        script_index = 0;
        while (morobox8_scripts[script_index].ext)
        {
            morobox8_cart_load_code(cart, path, (morobox8_u8)i, morobox8_scripts[script_index].ext, morobox8_scripts[script_index].lang);
            script_index++;
        }
    }
    morobox8_cart_select_tileset(cart, 0);
    morobox8_cart_select_font(cart, 1);
    morobox8_cart_select_code(cart, 0);
    morobox8_printf("dir loaded\n");
}

MOROBOX8_PUBLIC(void)
morobox8_cart_dump_dir(morobox8_cart *cart, const char *path)
{
}

MOROBOX8_PUBLIC(void)
morobox8_cart_load(morobox8_cart *cart, const void *buf, size_t size)
{
    size_t cart_size = sizeof(morobox8_cart);
    memset(cart, 0, cart_size);

    if (size < cart_size)
    {
        cart_size = size;
    }

    memcpy((void *)cart, buf, cart_size);
}

MOROBOX8_PUBLIC(size_t)
morobox8_cart_dump(morobox8_cart *cart, void *buf, size_t size)
{
    size_t cart_size = sizeof(morobox8_cart);

    if (size < cart_size)
    {
        cart_size = size;
    }

    memcpy(buf, (void *)cart, cart_size);
    return cart_size;
}
#endif

static morobox8_cart_chunk *morobox8_card_find_bank(morobox8_cart *cart, morobox8_cart_chunk_type type, morobox8_u8 id)
{
    morobox8_cart_chunk *chunk = cart->chunks;
    while (chunk)
    {
        if (chunk->type == type && chunk->id == id)
        {
            return chunk;
        }

        chunk = chunk->next;
    }

    return NULL;
}

MOROBOX8_PUBLIC(void)
morobox8_cart_select_font(morobox8_cart *cart, morobox8_u8 id)
{
    morobox8_cart_chunk *chunk = morobox8_card_find_bank(cart, MOROBOX8_CART_CHUNK_TILESET, id);
    if (!chunk)
    {
        return;
    }

    memcpy(&cart->data.font, &((morobox8_cart_tileset_chunk *)chunk)->tileset, sizeof(morobox8_cart_tileset));
    morobox8_printf("font bank %u selected\n", id);
}

MOROBOX8_PUBLIC(void)
morobox8_cart_select_tileset(morobox8_cart *cart, morobox8_u8 id)
{
    morobox8_cart_chunk *chunk = morobox8_card_find_bank(cart, MOROBOX8_CART_CHUNK_TILESET, id);
    if (!chunk)
    {
        return;
    }

    memcpy(&cart->data.tileset, &((morobox8_cart_tileset_chunk *)chunk)->tileset, sizeof(morobox8_cart_tileset));
    morobox8_printf("tileset bank %u selected\n", id);
}

MOROBOX8_PUBLIC(void)
morobox8_cart_select_code(morobox8_cart *cart, morobox8_u8 id)
{
    morobox8_cart_chunk *chunk = morobox8_card_find_bank(cart, MOROBOX8_CART_CHUNK_CODE, id);
    if (!chunk)
    {
        return;
    }

    memcpy(&cart->data.code, &((morobox8_cart_code_chunk *)chunk)->code, sizeof(morobox8_cart_code));
    morobox8_printf("code bank %u selected\n", id);
}
