#include "test_config.h"
#include "tool/unpacker.h"

typedef struct morobox8_cart_chunk morobox8_cart_chunk;
typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_64k_cart morobox8_64k_cart;
typedef struct morobox8_cart_any_chunk morobox8_cart_any_chunk;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_unpacker morobox8_unpacker;
typedef struct morobox8_unpacker_chunks_iterator morobox8_unpacker_chunks_iterator;

static morobox8_cart TEST_CART = {
    .header = {
        .magic_code = MOROBOX8_CART_MAGIC_CODE,
        .name = "test",
        .size = sizeof(morobox8_cart),
        .chunks = {
            {.id = 0,
             .name = MOROBOX8_LUA_MAIN,
             .type = MOROBOX8_CART_CHUNK_CODE,
             .address = 1},
            {.id = 1,
             .name = "tileset.png",
             .type = MOROBOX8_CART_CHUNK_TILESET,
             .address = 2},
            {.id = 2,
             .name = MOROBOX8_DEFAULT_FONT_NAME,
             .type = MOROBOX8_CART_CHUNK_TILESET,
             .address = 3},
            {.id = 0,
             .name = "",
             .type = 0,
             .address = 0}},
        .num_chunks = 3,
        .code_chunk = 0,
        .tileset_chunk = 1,
        .font_chunk = 2},
    .data = {0}};

/** Test read header */
static void test_read_header(void **state)
{
    morobox8_cart_header header;
    morobox8_unpacker *unpacker = morobox8_memory_unpacker(&TEST_CART, TEST_CART.header.size);
    assert_true(morobox8_unpacker_read_header(unpacker, &header));
    assert_cart_header_equal(&header, &TEST_CART.header);
    morobox8_unpacker_delete(unpacker);
}

/** Test read chunks */
static void test_read_chunks(void **state)
{
    size_t i = 0;
    morobox8_cart_any_chunk chunk;
    morobox8_unpacker *unpacker = morobox8_memory_unpacker(&TEST_CART, TEST_CART.header.size);
    while (TEST_CART.header.chunks[i].type != 0)
    {
        assert_true(morobox8_unpacker_read_chunk(unpacker, i, &chunk));
        assert_cart_chunk_equal(&chunk.code.base, &TEST_CART.header.chunks[i]);
        ++i;
    }
    morobox8_unpacker_delete(unpacker);
}

/** Test iterate chunks */
static void test_iter_chunks(void **state)
{
    size_t i = 0;
    morobox8_unpacker *unpacker = morobox8_memory_unpacker(&TEST_CART, TEST_CART.header.size);
    morobox8_unpacker_chunks_iterator *it = morobox8_unpacker_open_chunks(unpacker);
    while ((it = morobox8_unpacker_iter_chunks(it)) != NULL)
    {
        assert_true(i < TEST_CART.header.num_chunks);
        assert_cart_chunk_equal(&it->chunk.code.base, &TEST_CART.header.chunks[i]);
        ++i;
    }
    assert_int_equal(i, TEST_CART.header.num_chunks);
    morobox8_unpacker_close_chunks(it);
    morobox8_unpacker_delete(unpacker);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_read_header),
        cmocka_unit_test(test_read_chunks),
        cmocka_unit_test(test_iter_chunks)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
