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
typedef struct morobox8_cart_sprite morobox8_cart_sprite;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart morobox8_cart;

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

struct morobox8_packer;
typedef struct morobox8_packer
{
    /* Function to write data to output. */
    void (*write)(struct morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size);
    /* Output to write data to. */
    void *output;
    /* Generated cart. */
    morobox8_cart cart;
    /* Number of chunks. */
    size_t num_chunks;
    /* Size of all chunks. */
    size_t chunks_size;
} morobox8_packer;

struct morobox8_memory_packer
{
    /* Base packer. */
    morobox8_packer base;
    /* Size of output buffer. */
    size_t size;
};

MOROBOX8_CID_C(morobox8_packer)

static struct morobox8_memory_packer *morobox8_memory_packer_create(void)
{
    struct morobox8_memory_packer *o = (struct morobox8_memory_packer *)morobox8_malloc(sizeof(struct morobox8_memory_packer));
    if (o)
    {
        memset(o, 0, sizeof(struct morobox8_memory_packer));
    }

    return o;
}

static inline void morobox8_packer_write(morobox8_packer *packer, const void *buf, size_t offset, size_t size)
{
    if (packer->write)
    {
        packer->write(packer, packer->output, buf, offset, size);
    }
}

MOROBOX8_PUBLIC(void)
morobox8_packer_set_writer(morobox8_packer *packer, void (*write)(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size))
{
    packer->write = write;
}

MOROBOX8_PUBLIC(void)
morobox8_packer_set_output(morobox8_packer *packer, void *output)
{
    packer->output = output;
}

static void morobox8_packer_write_memory(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size)
{
    size_t output_size = ((struct morobox8_memory_packer *)packer)->size;
    if (offset >= output_size)
    {
        return;
    }

    memcpy((void *)&((morobox8_u8 *)output)[offset], buf, min(size, output_size - offset));
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

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packer_get_num_colors(morobox8_packer *packer)
{
    return packer->cart.data.num_colors;
}

#if MOROBOX8_FILESYSTEM
static void morobox8_packer_write_file(morobox8_packer *packer, void *output, const void *buf, size_t offset, size_t size)
{
    fseek((FILE *)output, offset, SEEK_SET);
    fwrite(buf, 1, size, (FILE *)output);
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
#endif

/* Loaders for files.*/
static struct
{
    const char *ext;
    size_t ext_size;
    int (*load)(morobox8_packer *packer, const char *name, const void *buf, size_t size);
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

MOROBOX8_PUBLIC(int)
morobox8_packer_add_file(morobox8_packer *packer, const char *name, const void *buf, size_t size)
{
    // Name must contain the extension to determine file type
    const char *pext = strrchr(name, '.');
    if (!pext)
    {
        morobox8_printf("Missing file extension %s\n", name);
        return -1;
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
    return -1;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_add_color(morobox8_packer *packer, morobox8_u8 r, morobox8_u8 g, morobox8_u8 b)
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

    return -1;
}

static int morobox8_packer_add_chunk(morobox8_packer *packer, const morobox8_cart_chunk *chunk, const void *data, size_t size)
{
    if (packer->num_chunks >= MOROBOX8_MAX_CHUNKS)
    {
        return -1;
    }

    memcpy(&packer->cart.header.chunks[packer->num_chunks], chunk, sizeof(morobox8_cart_chunk));
    packer->cart.header.chunks[packer->num_chunks].chunk.id = packer->num_chunks;
    packer->cart.header.chunks[packer->num_chunks].address = sizeof(morobox8_cart) + packer->chunks_size;
    morobox8_packer_write(packer, data, packer->cart.header.chunks[packer->num_chunks].address, size);
    packer->num_chunks++;
    packer->chunks_size += size;
    return packer->num_chunks;
}

MOROBOX8_PUBLIC(int)
morobox8_packer_add_code_chunk(morobox8_packer *packer, const morobox8_cart_code_chunk *chunk)
{
    return morobox8_packer_add_chunk(packer, &chunk->base, &chunk->code, sizeof(morobox8_cart_code));
}

MOROBOX8_PUBLIC(int)
morobox8_packer_add_tileset_chunk(morobox8_packer *packer, const morobox8_cart_tileset_chunk *chunk)
{
    return morobox8_packer_add_chunk(packer, &chunk->base, &chunk->tileset, sizeof(morobox8_cart_tileset));
}

MOROBOX8_PUBLIC(void)
morobox8_packer_pack(morobox8_packer *packer)
{
    morobox8_packer_write(packer, (void *)&packer->cart, 0, sizeof(morobox8_cart));
}

#endif
