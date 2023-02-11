#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmocka.h>
#include "tests/fs_testutils.h"
#include "fs.h"
#include "morobox8.h"
#include "cart/cart.h"
#include "tool/info.h"

typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8_cart_palette morobox8_cart_palette;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_max_size_cart morobox8_max_size_cart;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_code morobox8_cart_code;
typedef enum morobox8_info_level morobox8_info_level;

static void assert_cart_chunk_equal(morobox8_cart_chunk *chunk, const morobox8_cart_chunk *expected)
{
    assert_int_equal(chunk->id, expected->id);
    assert_int_equal(chunk->type, expected->type);
    assert_int_equal(chunk->address, expected->address);
    assert_string_equal(chunk->name, expected->name);
}

static void assert_cart_header_equal(morobox8_cart_header *header, const morobox8_cart_header *expected)
{
    assert_string_equal(header->magic_code, expected->magic_code);
    assert_string_equal(header->name, expected->name);
    assert_int_equal(header->num_chunks, expected->num_chunks);
    for (size_t i = 0; i < header->num_chunks; ++i)
    {
        assert_cart_chunk_equal(&header->chunks[i], &expected->chunks[i]);
    }
    assert_int_equal(header->size, expected->size);
    assert_int_equal(header->code_chunk, expected->code_chunk);
    assert_int_equal(header->tileset_chunk, expected->tileset_chunk);
    assert_int_equal(header->font_chunk, expected->font_chunk);
}

static void assert_cart_equal(morobox8_cart *cart, const morobox8_cart *expected)
{
    if (!cart)
    {
        assert_null(expected);
        return;
    }

    assert_non_null(expected);
    assert_memory_equal(&cart->header.magic_code[0], &expected->header.magic_code[0], MOROBOX8_CART_MAGIC_CODE_SIZE);
    assert_memory_equal(&cart->header.name[0], &expected->header.name[0], MOROBOX8_CARTNAME_SIZE);
    assert_int_equal(cart->header.size, expected->header.size);
    assert_int_equal(cart->header.num_chunks, expected->header.num_chunks);
    assert_int_equal(cart->header.code_chunk, expected->header.code_chunk);
    assert_int_equal(cart->header.tileset_chunk, expected->header.tileset_chunk);
    assert_int_equal(cart->header.font_chunk, expected->header.font_chunk);
    assert_int_equal(cart->header.num_colors, expected->header.num_colors);
    assert_memory_equal(&cart->header, &expected->header, sizeof(morobox8_cart_header));

    assert_memory_equal(&cart->data.palette, &expected->data.palette, sizeof(morobox8_cart_palette));
    assert_memory_equal(&cart->data.tileset, &expected->data.tileset, sizeof(morobox8_cart_tileset));
    assert_memory_equal(&cart->data.font, &expected->data.font, sizeof(morobox8_cart_tileset));
    assert_memory_equal(&cart->data.code, &expected->data.code, sizeof(morobox8_cart_code));
}

static void assert_cart_info_level(morobox8_cart *cart, const char *expected, morobox8_info_level level)
{
    char cart_buf[MOROBOX8_INFO_SIZE];
    size_t cart_size;
    assert_true(morobox8_info_cart(cart, &cart_buf[0], MOROBOX8_INFO_SIZE, &cart_size, level));

    char cwd[LIBFS_MAX_PATH];
    fs_assert_current_dir(&cwd);

    char buf[LIBFS_MAX_PATH];
    fs_assert_join_path(&buf, cwd, expected);

    size_t file_size;
    void *file_buf = fs_read_file(buf, &file_size);
    assert_non_null(file_buf);
    assert_string_equal(cart_buf, (char *)file_buf);
}

#define assert_cart_info(cart, expected) assert_cart_info_level(cart, expected, MOROBOX8_INFO_VERBOSE);
#define assert_cart_info_full(cart, expected) assert_cart_info_level(cart, expected, MOROBOX8_INFO_EXTRA_VERBOSE);