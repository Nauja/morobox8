#include "tool/pack.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "api/lua_api.h"
#include "api/js_api.h"
#include "cart/cart.h"

#if MOROBOX8_FILESYSTEM
#include "fs.h"
#include "png.h"

typedef struct fs_directory_iterator fs_directory_iterator;
#endif

typedef struct morobox8_packer morobox8_packer;

typedef struct morobox8_api_config morobox8_api_config;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart morobox8_cart;

typedef enum morobox8_api_lang morobox8_api_lang;

#define min(a, b) (a < b ? a : b)

struct morobox8_packer_chunk;
typedef struct morobox8_packer_chunk
{
    /* Chunk data. */
    union
    {
        /* This is either a tileset. */
        struct morobox8_cart_tileset_chunk tileset_chunk;
        /* Or code. */
        struct morobox8_cart_code_chunk code_chunk;
    } data;
    /* Pointer to next chunk. */
    struct morobox8_packer_chunk *next;
} morobox8_packer_chunk;

typedef struct morobox8_packer
{
    /* Number of chunks. */
    size_t num_chunks;
    /* Size of all chunks. */
    size_t chunks_size;
    /* List of chunks. */
    morobox8_packer_chunk chunks;
    /* Pointer to last chunk. */
    morobox8_packer_chunk *last_chunk;
    /* Generated cart. */
    morobox8_cart cart;
} morobox8_packer;

MOROBOX8_CID_C(morobox8_packer)

static morobox8_packer_chunk *morobox8_packer_add_chunk(morobox8_packer *packer, const char *name)
{
    morobox8_packer_chunk *o = (morobox8_packer_chunk *)morobox8_malloc(sizeof(morobox8_packer_chunk));
    if (!o)
    {
        return NULL;
    }

    memset(o, 0, sizeof(morobox8_packer_chunk));
    memcpy(o->data.code_chunk.base.name, name, strlen(name));
    o->data.code_chunk.base.id = packer->num_chunks;
    packer->num_chunks++;

    if (!packer->last_chunk)
    {
        packer->chunks.next = o;
    }
    else
    {
        packer->last_chunk->next = o;
    }

    packer->last_chunk = o;
    return o;
}

static morobox8_packer_chunk *morobox8_packer_add_code_chunk(morobox8_packer *packer, const char *name, const morobox8_api_config *config)
{
    morobox8_packer_chunk *o = morobox8_packer_add_chunk(packer, name);
    if (o)
    {
        o->data.code_chunk.base.type = MOROBOX8_CART_CHUNK_CODE;
        o->data.code_chunk.code.lang = config->lang;
    }

    return o;
}

static morobox8_packer_chunk *morobox8_packer_add_tileset_chunk(morobox8_packer *packer, const char *name)
{
    morobox8_packer_chunk *o = morobox8_packer_add_chunk(packer, name);
    if (o)
    {
        o->data.code_chunk.base.type = MOROBOX8_CART_CHUNK_TILESET;
    }

    return o;
}

static morobox8_u8 morobox8_packer_palette_add_color(morobox8_packer *packer, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    morobox8_cart *cart = &packer->cart;
    morobox8_cart_color *color = &cart->data.palette[0];
    for (morobox8_u8 i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        if (i >= cart->data.num_colors)
        {
            color->r = r;
            color->g = g;
            color->b = b;
            cart->data.num_colors++;
            morobox8_printf("New color (%u, %u, %u) with index %d\n", r, g, b, i);
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

static morobox8_u8 morobox8_packer_sprite_add_color(morobox8_packer *packer, morobox8_cart_sprite *sprite, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    morobox8_u8 id = morobox8_packer_palette_add_color(packer, r, g, b);
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

static void morobox8_packer_load_tileset(morobox8_packer *packer, morobox8_cart_tileset *tileset, morobox8_u8 *data, size_t width, size_t height)
{
    morobox8_u8 *pdata;
    morobox8_cart_sprite *sprite = &tileset->sprites[0];
    morobox8_u8 *pixels;
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
                    *pixels = morobox8_packer_sprite_add_color(
                        packer,
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

static morobox8_packer_chunk *morobox8_packer_load_png(morobox8_packer *packer, const char *name, void *buf, size_t size)
{
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

    morobox8_packer_chunk *chunk = morobox8_packer_add_tileset_chunk(packer, name);
    if (!chunk)
    {
        morobox8_free(data);
        return NULL;
    }

    morobox8_printf("Found %ux%u sprites\n", tiles_w, tiles_h);
    size_t num_colors = packer->cart.data.num_colors;
    morobox8_packer_load_tileset(packer, &chunk->data.tileset_chunk.tileset, data, tiles_w, tiles_h);
    morobox8_printf("Indexed %d new colors, total %d\n", packer->cart.data.num_colors - num_colors, packer->cart.data.num_colors);

    png_destroy_read_struct(&png, &info, NULL);
    morobox8_free(data);
    return chunk;
}

static morobox8_packer_chunk *morobox8_packer_load_code(morobox8_packer *packer, const char *name, void *buf, size_t size, const morobox8_api_config *config)
{
    morobox8_packer_chunk *chunk = morobox8_packer_add_code_chunk(packer, name, config);
    if (!chunk)
    {
        return NULL;
    }

    memcpy(&chunk->data.code_chunk.code.text[0], buf, min(MOROBOX8_CART_CODE_SIZE, size));
    morobox8_printf("Script takes %d%% space\n", (int)((size / (float)MOROBOX8_CART_CODE_SIZE) * 100.0f));
    morobox8_printf("%d bytes used\n", size);

    if (strncmp(name, config->main, config->main_size) == 0)
    {
        memcpy(&packer->cart.data.code, &chunk->data.code_chunk.code, sizeof(morobox8_cart_code));
        morobox8_printf("Select main script\n");
    }

    return chunk;
}

#if MOROBOX8_FILESYSTEM
MOROBOX8_PUBLIC(void)
morobox8_packer_add_dir(morobox8_packer *packer, const char *path)
{
    fs_directory_iterator *it = fs_open_dir(path);
    if (!it)
    {
        return;
    }

    char file_path[MOROBOX8_FILENAME_SIZE];
    size_t size;
    while (fs_read_dir(it))
    {
        fs_join_path(&file_path[0], MOROBOX8_FILENAME_SIZE, path, it->path);
        void *buf = fs_read_file(&file_path[0], &size);
        morobox8_packer_add_file(packer, it->path, buf, size);
        morobox8_free(buf);
    }

    fs_close_dir(it);
}

MOROBOX8_PUBLIC(struct morobox8_cart *)
morobox8_pack(const char *path, struct morobox8_cart *cart)
{
    morobox8_packer packer;
    morobox8_packer_init(&packer);
    morobox8_packer_add_dir(&packer, path);
    morobox8_packer_pack(&packer, cart);
    return cart;
}
#endif

/* Loaders for assets types.*/
static struct
{
    const char *ext;
    morobox8_packer_chunk *(*load)(morobox8_packer *packer, const char *name, void *buf, size_t size);
} morobox8_assets_loaders[] = {
    {".png", morobox8_packer_load_png},
    {NULL, NULL},
};

/* Configurations of the API. */
static const morobox8_api_config *morobox8_api_configs[] = {
#if MOROBOX8_LUA_API
    &morobox8_lua_api_config,
#endif
#if MOROBOX8_JS_API
    &morobox8_js_api_config,
#endif
    NULL};

static int morobox8_packer_add_asset(morobox8_packer *packer, const char *name, void *buf, size_t size)
{
    size_t i = 0;
    const char *pext;
    morobox8_packer_chunk *chunk;
    while (morobox8_assets_loaders[i].ext)
    {
        pext = strrchr(name, '.');
        if (pext && strncmp(pext, morobox8_assets_loaders[i].ext, 4) == 0)
        {
            morobox8_printf("Pack %s...\n", name);
            if ((chunk = morobox8_assets_loaders[i].load(packer, name, buf, size)) != NULL)
            {
                morobox8_printf("Packed %s in bank %d\n", name, chunk->data.code_chunk.base.id);
            }
            else
            {
                morobox8_printf("Failed to pack %s\n", name);
            }
            return MOROBOX8_TRUE;
        }
        ++i;
    }

    return MOROBOX8_FALSE;
}

static int morobox8_packer_add_code(morobox8_packer *packer, const char *name, void *buf, size_t size)
{
    size_t i = 0;
    const char *pext;
    morobox8_packer_chunk *chunk;
    const morobox8_api_config *config;
    while ((config = morobox8_api_configs[i]) != NULL)
    {
        pext = strrchr(name, '.');
        if (pext && strncmp(pext, config->ext, config->ext_size) == 0)
        {
            morobox8_printf("Pack %s...\n", name);
            if ((chunk = morobox8_packer_load_code(packer, name, buf, size, config)) != NULL)
            {
                morobox8_printf("Packed %s in bank %d\n", name, chunk->data.code_chunk.base.id);
            }
            else
            {
                morobox8_printf("Failed to pack %s\n", name);
            }
            return MOROBOX8_TRUE;
        }
        ++i;
    }

    return MOROBOX8_FALSE;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_add_file(morobox8_packer *packer, const char *name, void *buf, size_t size)
{
    if (morobox8_packer_add_asset(packer, name, buf, size))
    {
        return;
    }

    morobox8_packer_add_code(packer, name, buf, size);
}

MOROBOX8_PUBLIC(void)
morobox8_packer_pack(morobox8_packer *packer, morobox8_cart *cart)
{
    memcpy((void *)cart, (const void *)&packer->cart, sizeof(morobox8_cart));
    memcpy(&cart->header.magic_code[0], MOROBOX8_CART_MAGIC_CODE, MOROBOX8_CART_MAGIC_CODE_SIZE);
    cart->header.num_chunks = packer->num_chunks;
}
