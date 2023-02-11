#include "test_config.h"

typedef struct morobox8_cart morobox8_cart;

/** Check the size of a 64k cart */
static void test_64k_cart_size(void **state)
{
    assert_true(sizeof(morobox8_cart) == 0x10000);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_64k_cart_size)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
