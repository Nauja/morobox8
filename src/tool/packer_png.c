#include "tool/packer.h"

#if MOROBOX8_PACKER

#include "morobox8.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"
#include "cart/cart.h"

#if MOROBOX8_FILESYSTEM
#include "png.h"
#endif

typedef struct morobox8_packer morobox8_packer;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart morobox8_cart;

static int morobox8_packer_sprite_add_color(morobox8_packer *packer, morobox8_cart_sprite *sprite, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    int id = morobox8_packer_add_color(packer, r, g, b);
    if (id == -1)
    {
        return id;
    }

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

    return -1;
}

static void morobox8_packer_index_png(morobox8_packer *packer, morobox8_cart_tileset *tileset, morobox8_u8 *data, size_t width, size_t height)
{
    morobox8_u8 *pdata;
    morobox8_cart_sprite *sprite = &tileset->sprites[0];
    morobox8_u8 *pixels;
    int color;
    for (size_t tile_j = 0; tile_j < height; ++tile_j)
    {
        for (size_t tile_i = 0; tile_i < width; ++tile_i)
        {
            sprite->num_colors = 0;
            pixels = &sprite->pixels[0];
            pdata = &data[((tile_i * 8) + ((tile_j * 8) * width)) * 4];
            for (morobox8_u8 j = 0; j < MOROBOX8_SPRITE_HEIGHT; ++j)
            {
                for (morobox8_u8 i = 0; i < MOROBOX8_SPRITE_WIDTH; ++i)
                {
                    color = morobox8_packer_sprite_add_color(
                        packer,
                        sprite,
                        pdata[0],
                        pdata[1],
                        pdata[2]);
                    pdata += 4;
                    *pixels = color == -1 ? 0 : (morobox8_u8)color;
                    ++pixels;
                }
                pdata += (width * 4) - MOROBOX8_SPRITE_WIDTH * 4;
            }
            ++sprite;
        }
    }
}

typedef struct morobox8_packer_pio
{
    png_byte *buf;
    size_t size;
} morobox8_packer_pio;

static void morobox8_packer_png_from_memory(png_structp png, png_bytep buf, size_t size)
{
    morobox8_packer_pio *pio = (morobox8_packer_pio *)png_get_io_ptr(png);
    if (pio->size == 0)
    {
        return;
    }

    size_t read = min(size, pio->size);
    memcpy(buf, pio->buf, read);
    pio->buf = pio->buf + read;
    pio->size -= read;
}

static morobox8_u8 morobox8_packer_load_png(morobox8_packer *packer, morobox8_cart_tileset_chunk *chunk, const char *name, const void *buf, size_t size)
{
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        return MOROBOX8_CHUNK_INVALID;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        return MOROBOX8_CHUNK_INVALID;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        return MOROBOX8_CHUNK_INVALID;
    }

    morobox8_packer_pio pio = {
        .buf = ((png_byte *)buf),
        .size = size};

    png_set_read_fn(png, &pio, &morobox8_packer_png_from_memory);
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

    morobox8_u8 *data = morobox8_malloc(sizeof(morobox8_u32) * width * height);
    png_bytep *rows = (png_bytep *)morobox8_malloc(sizeof(png_bytep) * height);

    for (int i = 0; i < height; i++)
        rows[i] = data + width * i * sizeof(morobox8_u32);

    png_read_image(png, rows);

    morobox8_free(rows);

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

    morobox8_u8 result = morobox8_packer_add_tileset_chunk(packer, chunk);
    morobox8_packer_index_png(packer, &chunk->tileset, data, tiles_w, tiles_h);

    png_destroy_read_struct(&png, &info, NULL);
    morobox8_free(data);
    return result;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_png(morobox8_packer *packer, const char *name, const void *buf, size_t size)
{
    morobox8_cart_tileset_chunk chunk;
    morobox8_cart_tileset_chunk_init(&chunk);
    snprintf(chunk.base.name, MOROBOX8_CHUNK_NAME_SIZE, "%s", name);
    return morobox8_packer_load_png(packer, &chunk, name, buf, size);
}

#endif
