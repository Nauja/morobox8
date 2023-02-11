#include "test_config.h"
#include "tool/packer.h"

#define MOROBOX8_CART_DATA_DIR "data/cart"
#define MOROBOX8_CART_CODE_FILE "data/cart/main.lua"
#define MOROBOX8_CART_TILESET_FILE "data/cart/tileset.png"
#define MOROBOX8_CART_FONT_FILE "data/cart/font.png"
#define MOROBOX8_EMPTY_CART_TXT "expected/empty_cart.txt"
#define MOROBOX8_ONE_CODE_CHUNK_CART_TXT "expected/one_code_chunk_cart.txt"
#define MOROBOX8_ONE_TILESET_CHUNK_CART_TXT "expected/one_tileset_chunk_cart.txt"
#define MOROBOX8_ONE_FONT_CHUNK_CART_TXT "expected/one_font_chunk_cart.txt"
#define MOROBOX8_64K_CART_TXT "expected/64k_cart.txt"
#define MOROBOX8_256K_CART_TXT "expected/256k_cart.txt"
#define MOROBOX8_DATA_DIR_CART_TXT "expected/data_dir_cart.txt"
#define MOROBOX8_DATA_DIR_CART_FULL_TXT "expected/data_dir_cart_full.txt"
#define MOROBOX8_CART_NAME "test"

typedef struct morobox8_cart_header morobox8_cart_header;
typedef struct morobox8_cart_data morobox8_cart_data;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_64k_cart morobox8_64k_cart;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_cart_tileset morobox8_cart_tileset;
typedef struct morobox8_cart_code_chunk morobox8_cart_code_chunk;
typedef struct morobox8_cart_tileset_chunk morobox8_cart_tileset_chunk;
typedef struct morobox8_packer morobox8_packer;

static const morobox8_cart_code_chunk CODE_CHUNK = {
    .base = {
        .id = 0,
        .name = MOROBOX8_LUA_MAIN,
        .type = MOROBOX8_CART_CHUNK_CODE},
    .code = {.lang = MOROBOX8_API_LANG_LUA, .text = "function tick()\n\
    print(\"tick\")\n\
end"}};

static const morobox8_cart_tileset_chunk TILESET_CHUNK = {
    .base = {
        .id = 0,
        .name = "tileset.png",
        .type = MOROBOX8_CART_CHUNK_TILESET},
    .tileset = {.sprites = {0}}};

static const morobox8_cart_tileset_chunk FONT_CHUNK = {
    .base = {
        .id = 0,
        .name = MOROBOX8_DEFAULT_FONT_NAME,
        .type = MOROBOX8_CART_CHUNK_TILESET},
    .tileset = {.sprites = {0}}};

static const morobox8_cart EXPECTED_EMPTY_CART = {
    .header = {
        .magic_code = MOROBOX8_CART_MAGIC_CODE,
        .name = "",
        .size = sizeof(morobox8_cart),
        .chunks = {0},
        .num_chunks = 0},
    .data = {0}};

/** Test packing a directory to a memory buffer */
static void test_pack_empty(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_EMPTY_CART_TXT);
}

/** Test adding colors */
static void test_add_color(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    assert_int_equal(morobox8_packer_get_num_colors(packer), 0);
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Fill the palette
    for (size_t i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        assert_int_equal(morobox8_packer_add_color(packer, i, 0, 0), i);
        assert_int_equal(morobox8_packer_get_num_colors(packer), i + 1);
    }

    // Palette is already full
    assert_int_equal(morobox8_packer_add_color(packer, MOROBOX8_PALETTE_SIZE, 0, 0), -1);
    assert_int_equal(morobox8_packer_get_num_colors(packer), MOROBOX8_PALETTE_SIZE);

    // Check colors are returned
    for (size_t i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        assert_int_equal(morobox8_packer_add_color(packer, i, 0, 0), i);
    }
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);

    // Check exported colors
    assert_int_equal(cart.header.num_colors, MOROBOX8_PALETTE_SIZE);
    for (size_t i = 0; i < MOROBOX8_PALETTE_SIZE; ++i)
    {
        assert_int_equal(cart.data.palette.colors[i].r, i);
        assert_int_equal(cart.data.palette.colors[i].g, 0);
        assert_int_equal(cart.data.palette.colors[i].b, 0);
    }
}

/** Test adding code */
static void test_add_code_chunk(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add code to the cart
    assert_int_equal(morobox8_packer_add_code_chunk(packer, &CODE_CHUNK), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_ONE_CODE_CHUNK_CART_TXT);
}

/** Test adding tileset */
static void test_add_tileset_chunk(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add tileset to the cart
    assert_int_equal(morobox8_packer_add_tileset_chunk(packer, &TILESET_CHUNK), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    cart.header.num_colors = 6;
    assert_cart_info(&cart, MOROBOX8_ONE_TILESET_CHUNK_CART_TXT);
}

/** Test adding font */
static void test_add_font_chunk(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add font to the cart
    assert_int_equal(morobox8_packer_add_tileset_chunk(packer, &FONT_CHUNK), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    cart.header.num_colors = 1;
    assert_cart_info(&cart, MOROBOX8_ONE_FONT_CHUNK_CART_TXT);
}

/** Test adding code directly */
static void test_add_code(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add code to the cart
    assert_int_equal(morobox8_packer_add_code(packer, CODE_CHUNK.base.name, CODE_CHUNK.code.text, strlen(CODE_CHUNK.code.text)), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_ONE_CODE_CHUNK_CART_TXT);
}

/** Test adding code from file */
static void test_add_code_file(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add code to the cart
    assert_int_equal(morobox8_packer_add_file(packer, MOROBOX8_CART_CODE_FILE), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_ONE_CODE_CHUNK_CART_TXT);
}

/** Test adding tileset from file */
static void test_add_tileset_file(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add code to the cart
    assert_int_equal(morobox8_packer_add_file(packer, MOROBOX8_CART_TILESET_FILE), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_ONE_TILESET_CHUNK_CART_TXT);
}

/** Test adding font from file */
static void test_add_font_file(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add code to the cart
    assert_int_equal(morobox8_packer_add_file(packer, MOROBOX8_CART_FONT_FILE), 0);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_ONE_FONT_CHUNK_CART_TXT);
}

/** Test a 64k cart with code, tileset and font */
static void test_64k_cart(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add chunks
    assert_int_equal(morobox8_packer_add_code_chunk(packer, &CODE_CHUNK), 0);
    assert_int_equal(morobox8_packer_add_tileset_chunk(packer, &TILESET_CHUNK), 1);
    assert_int_equal(morobox8_packer_add_tileset_chunk(packer, &FONT_CHUNK), 2);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    cart.header.num_colors = 6;
    assert_cart_info(&cart, MOROBOX8_64K_CART_TXT);
}

/** Test a full cart */
static void test_256k_cart(void **state)
{
    morobox8_cart cart;
    morobox8_64k_cart_init(&cart);
    morobox8_packer *packer = morobox8_memory_packer(&cart, sizeof(morobox8_cart));
    morobox8_packer_set_name(packer, MOROBOX8_CART_NAME);

    // Add chunks
    for (size_t i = 0; i < MOROBOX8_256K_MAX_CHUNKS; ++i)
    {
        assert_int_equal(morobox8_packer_add_code_chunk(packer, &CODE_CHUNK), i);
    }
    assert_int_equal(morobox8_packer_add_code_chunk(packer, &CODE_CHUNK), -1);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    assert_cart_info(&cart, MOROBOX8_256K_CART_TXT);
}

/** Test packing a directory to a memory buffer */
static void test_pack_to_memory(void **state)
{
    morobox8_cart cart;
    morobox8_pack_to_memory(MOROBOX8_CART_DATA_DIR, &cart, sizeof(morobox8_cart));
    assert_cart_info(&cart, MOROBOX8_DATA_DIR_CART_TXT);
}

/** Test packing a directory to a cart */
static void test_pack_to_cart(void **state)
{
    morobox8_cart cart;
    morobox8_pack_to_cart(MOROBOX8_CART_DATA_DIR, &cart);
    assert_cart_info(&cart, MOROBOX8_DATA_DIR_CART_TXT);
}

/** Test with full info */
static void test_extra_verbose(void **state)
{
    morobox8_cart cart;
    morobox8_pack_to_cart(MOROBOX8_CART_DATA_DIR, &cart);
    assert_cart_info_full(&cart, MOROBOX8_DATA_DIR_CART_FULL_TXT);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pack_empty),
        cmocka_unit_test(test_add_color),
        cmocka_unit_test(test_add_code_chunk),
        cmocka_unit_test(test_add_tileset_chunk),
        cmocka_unit_test(test_add_font_chunk),
        cmocka_unit_test(test_add_code),
        cmocka_unit_test(test_add_code_file),
        cmocka_unit_test(test_add_tileset_file),
        cmocka_unit_test(test_add_font_file),
        cmocka_unit_test(test_64k_cart),
        cmocka_unit_test(test_256k_cart),
        cmocka_unit_test(test_pack_to_memory),
        cmocka_unit_test(test_pack_to_cart),
        cmocka_unit_test(test_extra_verbose)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
