#include "tool/packer.h"

#if MOROBOX8_PACKER

#include "morobox8.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"
#include "api/lua_api.h"
#include "api/js_api.h"
#include "cart/cart.h"

#if MOROBOX8_FILESYSTEM
#include "fs.h"

typedef struct fs_directory_iterator fs_directory_iterator;
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_packer morobox8_packer;

typedef struct morobox8_api_config morobox8_api_config;
typedef struct morobox8_cart_color morobox8_cart_color;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_max_size_cart morobox8_max_size_cart;

struct morobox8_packer;
typedef struct morobox8_packer
{
    /* Function to write data to output. */
    size_t (*write)(struct morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size);
    /* Output to write data to. */
    void *output;
    /* Generated cart. */
    morobox8_cart cart;
    /* Number of chunks. */
    size_t num_chunks;
    /* Size of all chunks. */
    size_t chunks_size;
    /* Virtual size used to calcute chunks addresses. */
    size_t virtual_chunks_size;
} morobox8_packer;

struct morobox8_memory_packer
{
    /* Base packer. */
    morobox8_packer base;
    /* Size of output buffer. */
    size_t size;
};

MOROBOX8_PUBLIC(void)
morobox8_packer_init(morobox8_packer *packer)
{
    memset(packer, 0, sizeof(morobox8_packer));
    morobox8_64k_cart_init(&packer->cart);
}

MOROBOX8_CREATE_C(morobox8_packer)
MOROBOX8_DELETE_C(morobox8_packer)

static struct morobox8_memory_packer *morobox8_memory_packer_create(void)
{
    struct morobox8_memory_packer *o = (struct morobox8_memory_packer *)morobox8_malloc(sizeof(struct morobox8_memory_packer));
    if (o)
    {
        memset(o, 0, sizeof(struct morobox8_memory_packer));
        morobox8_packer_init(&o->base);
    }

    return o;
}

static inline size_t morobox8_packer_write(morobox8_packer *packer, const void *buf, size_t offset, size_t size)
{
    if (packer->write)
    {
        return packer->write(packer, packer->output, buf, offset, size);
    }

    return 0;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_set_writer(morobox8_packer *packer, size_t (*write)(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size))
{
    packer->write = write;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_set_output(morobox8_packer *packer, void *output)
{
    packer->output = output;
}

static size_t morobox8_packer_write_memory(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size)
{
    size_t output_size = ((struct morobox8_memory_packer *)packer)->size;
    if (offset >= output_size)
    {
        return 0;
    }

    size_t len = min(size, output_size - offset);
    memcpy((void *)&((morobox8_u8 *)output)[offset], buf, len);
    return len;
}

MOROBOX8_PUBLIC(morobox8_packer *)
morobox8_memory_packer(void *buf, size_t size)
{
    struct morobox8_memory_packer *packer = morobox8_memory_packer_create();
    if (packer)
    {
        packer->base.write = &morobox8_packer_write_memory;
        packer->base.output = buf;
        packer->size = size;
    }

    return &packer->base;
}

MOROBOX8_PUBLIC(const char *)
morobox8_packer_get_name(const morobox8_packer *packer)
{
    return packer->cart.header.name;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_set_name(morobox8_packer *packer, const char *name)
{
    memset(packer->cart.header.name, 0, MOROBOX8_CARTNAME_SIZE);
    snprintf(packer->cart.header.name, MOROBOX8_CARTNAME_SIZE, "%s", name);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_get_num_colors(const morobox8_packer *packer)
{
    return packer->cart.header.num_colors;
}

#if MOROBOX8_FILESYSTEM
static size_t morobox8_packer_write_file(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size)
{
    fseek((FILE *)output, offset, SEEK_SET);
    return fwrite(buf, 1, size, (FILE *)output);
}

MOROBOX8_PUBLIC(morobox8_packer *)
morobox8_file_packer(FILE *file)
{
    morobox8_packer *packer = morobox8_packer_create();
    if (packer)
    {
        packer->write = &morobox8_packer_write_file;
        packer->output = (void *)file;
    }

    return packer;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_add_dir(morobox8_packer *packer, const char *path)
{
    fs_directory_iterator *it = fs_open_dir(path);
    if (!it)
    {
        return MOROBOX8_FALSE;
    }

    // Take the name of the directory
    morobox8_packer_set_name(packer, fs_basename(path));

    char file_path[MOROBOX8_FILENAME_SIZE];
    while (fs_read_dir(it))
    {
        fs_join_path(&file_path[0], MOROBOX8_FILENAME_SIZE, path, it->path);
        morobox8_packer_add_file(packer, &file_path[0]);
    }

    fs_close_dir(it);
    return MOROBOX8_TRUE;
}
#endif

/* Loaders for files.*/
static struct
{
    const char *ext;
    size_t ext_size;
    morobox8_u8 (*load)(morobox8_packer *packer, const char *name, const void *buf, size_t size);
} morobox8_assets_loaders[] = {
    {".png", 4, &morobox8_packer_add_png},
#if MOROBOX8_LUA_API
    {".lua", 4, &morobox8_packer_add_code},
#endif
#if MOROBOX8_JS_API
    {".js", 3, &morobox8_packer_add_code},
#endif
    {NULL, 0, NULL},
};

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_file(morobox8_packer *packer, const char *path)
{
    size_t size;
    void *buf = fs_read_file(path, &size);
    if (!buf)
    {
        return MOROBOX8_CHUNK_INVALID;
    }

    morobox8_u8 result = morobox8_packer_add_memory(packer, fs_basename(path), buf, size);
    morobox8_free(buf);
    return result;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_memory(morobox8_packer *packer, const char *name, const void *buf, size_t size)
{
    // Name must contain the extension to determine file type
    const char *pext = strrchr(name, '.');
    if (!pext)
    {
        morobox8_printf("Missing file extension %s\n", name);
        return MOROBOX8_CHUNK_INVALID;
    }

    size_t i = 0;
    while (morobox8_assets_loaders[i].ext)
    {
        // Match the extension with possible file type
        if (strncmp(pext, morobox8_assets_loaders[i].ext, morobox8_assets_loaders[i].ext_size) == 0)
        {
            return morobox8_assets_loaders[i].load(packer, name, buf, size);
        }

        ++i;
    }

    morobox8_printf("Unknown file extension %s\n", name);
    return MOROBOX8_CHUNK_INVALID;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_add_color(morobox8_packer *packer, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
{
    morobox8_cart_header *header = &packer->cart.header;
    morobox8_cart_data *data = &packer->cart.data;
    morobox8_cart_color *color = &data->palette.colors[0];
    for (morobox8_u8 i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        if (i >= header->num_colors)
        {
            color->r = r;
            color->g = g;
            color->b = b;
            header->num_colors++;
            return i;
        }

        if (color->r != r || color->g != g || color->b != b)
        {
            ++color;
            continue;
        }

        return i;
    }

    return -1;
}

static morobox8_u8 morobox8_packer_add_chunk(morobox8_packer *packer, const morobox8_cart_chunk *chunk, const void *data, size_t size, morobox8_u8 type)
{
    if (packer->num_chunks >= MOROBOX8_MAX_CHUNKS)
    {
        return MOROBOX8_CHUNK_INVALID;
    }

    size_t chunk_index = packer->num_chunks;
    memcpy(&packer->cart.header.chunks[chunk_index], chunk, sizeof(morobox8_cart_chunk));
    packer->cart.header.chunks[chunk_index].id = chunk_index;
    packer->cart.header.chunks[chunk_index].type = type;
    packer->cart.header.chunks[chunk_index].address = sizeof(morobox8_cart) + packer->virtual_chunks_size;
    size_t len = morobox8_packer_write(packer, data, packer->cart.header.chunks[chunk_index].address, size);
    packer->chunks_size += len;
    packer->virtual_chunks_size += size;
    packer->num_chunks++;
    return chunk_index;
}

static int morobox8_packer_is_main(const morobox8_cart_code_chunk *chunk)
{
    size_t i = 0;
    const morobox8_api_config *config;
    while ((config = morobox8_api_configs[i]) != NULL)
    {
        if (strncmp(chunk->base.name, morobox8_api_configs[i]->main, morobox8_api_configs[i]->main_size) == 0)
        {
            return MOROBOX8_TRUE;
        }

        ++i;
    }

    return MOROBOX8_FALSE;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_code_chunk(morobox8_packer *packer, const morobox8_cart_code_chunk *chunk)
{
    morobox8_u8 result = morobox8_packer_add_chunk(packer, &chunk->base, &chunk->code, sizeof(morobox8_cart_code), MOROBOX8_CART_CHUNK_CODE);
    if (result == MOROBOX8_CHUNK_INVALID)
    {
        return result;
    }

    // Copy the main code if needed
    if (packer->cart.header.code_chunk == MOROBOX8_CHUNK_INVALID && morobox8_packer_is_main(chunk))
    {
        memcpy(&packer->cart.data.code, &chunk->code, sizeof(morobox8_cart_code));
        packer->cart.header.code_chunk = result;
    }

    return result;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_add_tileset_chunk(morobox8_packer *packer, const morobox8_cart_tileset_chunk *chunk)
{
    morobox8_u8 result = morobox8_packer_add_chunk(packer, &chunk->base, &chunk->tileset, sizeof(morobox8_cart_tileset), MOROBOX8_CART_CHUNK_TILESET);
    if (result == MOROBOX8_CHUNK_INVALID)
    {
        return result;
    }

    // Fill the font tile
    if (packer->cart.header.font_chunk == MOROBOX8_CHUNK_INVALID && strncmp(chunk->base.name, MOROBOX8_DEFAULT_FONT_NAME, MOROBOX8_DEFAULT_FONT_NAME_SIZE) == 0)
    {
        memcpy(&packer->cart.data.font, &chunk->tileset, sizeof(morobox8_cart_tileset));
        packer->cart.header.font_chunk = result;
        return result;
    }

    // Fill in the first tileset of the cart
    if (packer->cart.header.tileset_chunk == MOROBOX8_CHUNK_INVALID)
    {
        memcpy(&packer->cart.data.tileset, &chunk->tileset, sizeof(morobox8_cart_tileset));
        packer->cart.header.tileset_chunk = result;
    }

    return result;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_select_code(morobox8_packer *packer, morobox8_u8 id)
{
    return 0;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_select_tileset(morobox8_packer *packer, morobox8_u8 id)
{
    return 0;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_select_font(morobox8_packer *packer, morobox8_u8 id)
{
    return 0;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_pack(morobox8_packer *packer)
{
    memcpy(&packer->cart.header.magic_code[0], MOROBOX8_CART_MAGIC_CODE, MOROBOX8_MAGICCODE_SIZE);
    packer->cart.header.size = sizeof(morobox8_cart) + packer->chunks_size;
    packer->cart.header.num_chunks = (morobox8_u8)packer->num_chunks;
    morobox8_packer_write(packer, (void *)&packer->cart, 0, sizeof(morobox8_cart));
}

static int morobox8_pack_dir(const char *src, morobox8_packer *packer)
{
    if (!packer)
    {
        return MOROBOX8_FALSE;
    }

    int result = morobox8_packer_add_dir(packer, src);
    if (result)
    {
        morobox8_packer_pack(packer);
    }

    morobox8_packer_delete(packer);
    return result;
}

MOROBOX8_PUBLIC(int)
morobox8_pack_to_file(const char *src, const char *dst)
{
    FILE *f = fopen(dst, "w+");
    morobox8_packer *packer = morobox8_file_packer(f);
    int result = morobox8_pack_dir(src, packer);
    fclose(f);
    return result;
}

MOROBOX8_PUBLIC(int)
morobox8_pack_to_memory(const char *src, void *buf, size_t size)
{
    morobox8_packer *packer = morobox8_memory_packer(buf, size);
    return morobox8_pack_dir(src, packer);
}

MOROBOX8_PUBLIC(int)
morobox8_pack_to_cart(const char *src, morobox8_cart *cart)
{
    return morobox8_pack_to_memory(src, cart, cart->header.size);
}

#endif
