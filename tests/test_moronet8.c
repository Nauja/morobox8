#include "test_config.h"

#define MORONET8_DATA_DIR "data"

/** Test allocating a new vm */
static void test_create(void **state)
{
    moronet8 *vm = moronet8_assert_create();

    moronet8_delete(vm);
}

/** Test dumping a cart */
static void test_dump_cart(void **state)
{
    const char *code = "\
function tick()\n\
  print(\"hello\")\n\
end\n\n";
    moronet8_cart cart;

    moronet8_cart_init(&cart);
    cart.lang = MORONET8_LANG_LUA;
    memcpy((void *)&cart.code, code, strlen(code));

    fs_assert_make_dir(MORONET8_DATA_DIR);
    char buf[LIBFS_MAX_PATH];
    fs_join_path(buf, LIBFS_MAX_PATH, MORONET8_DATA_DIR, "cart.bin");
    moronet8_cart_dump_file(&cart, buf);
}

/** Test loading bios */
static void test_load_bios(void **state)
{
    moronet8 vm;
    moronet8_init(&vm);

    moronet8_cart cart;
    cart.lang = MORONET8_LANG_LUA;
    // cart.code = "function tick() print(\"hello\") end";
    moronet8_load_bios(&vm, &cart);

    moronet8_tick(&vm, 0);

    moronet8_unload_bios(&vm);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_dump_cart),
        cmocka_unit_test(test_load_bios)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}