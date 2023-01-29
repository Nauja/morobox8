#include "moronet8.h"
#include "fs.h"
#include "argparse.h"

#include <SDL.h>

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct moronet8_hooks moronet8_hooks;
typedef enum moronet8_session_state moronet8_session_state;
typedef struct moronet8_session moronet8_session;
typedef struct moronet8_cart moronet8_cart;
typedef struct moronet8 moronet8;

// clang-format off
#define MORONET8_CMD_PARAMS_LIST(macro)                                                     \
    macro(join,         char*,  STRING,     "",         "host ip:port")                     \
    macro(scale,        int,    INTEGER,    "=<int>",   "main window scale")                \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
    char *cart;
#define MORONET8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MORONET8_CMD_PARAMS_LIST(MORONET8_CMD_PARAMS_DEF)
#undef MORONET8_CMD_PARAMS_DEF
} moronet8_args;

static moronet8_args moronet8_parse_args(int argc, char **argv)
{
    static const char *const usage[] =
        {
            MORONET8_NAME " [cart] [options]",
            NULL,
        };

    moronet8_args args = {.scale = 2};

    struct argparse_option options[] =
        {
            OPT_HELP(),
#define MORONET8_CMD_PARAMS_DEF(name, ctype, type, post, help) OPT_##type('\0', #name, &args.name, help, NULL, 0, 0),
            MORONET8_CMD_PARAMS_LIST(MORONET8_CMD_PARAMS_DEF)
#undef MORONET8_CMD_PARAMS_DEF
                OPT_END(),
        };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\n" MORONET8_NAME " startup options:", NULL);
    argc = argparse_parse(&argparse, argc, (const char **)argv);

    if (argc == 1)
        args.cart = argv[0];

    return args;
}

int moronet8_run_player(moronet8 *vm);
moronet8_session *moronet8_session_host_impl(const char *host);
moronet8_session *moronet8_session_join_impl(const char *host);
void moronet8_session_delete_impl(moronet8_session *session);
void moronet8_session_broadcast_impl(moronet8_session *session, const void *buf, size_t size);
size_t moronet8_session_receive_impl(moronet8_session *session, void *buf, size_t size);
moronet8_session_state moronet8_session_state_get_impl(moronet8_session *session);
void moronet8_session_poll_impl(moronet8_session *session);

static void dump(moronet8_cart *cart, enum moronet8_api_type type)
{
    if (type == MORONET8_API_CART)
    {
        printf("dump cart\n");
    }
    else
    {
        printf("dump bios\n");
    }
    printf("cart size %d\n", sizeof(struct moronet8_cart_data));
    printf("cart num col %d\n", cart->data.num_colors);
    printf("code size %d\n", sizeof(struct moronet8_cart_code));
    memcpy(&cart->header.magic_code[0], (const void *)"MB8\n", 4);
    FILE *f = fopen(type == MORONET8_API_CART ? "cart.txt" : "bios.txt", "wb+");
    fwrite((void *)cart, 1, sizeof(struct moronet8_cart), f);
    fclose(f);
}

static void moronet8_printf_impl(const char *fmt, va_list args)
{
    vfprintf(stdout, fmt, args);
}

int main(int argc, char **argv)
{
    moronet8_args args = moronet8_parse_args(argc, argv);

    moronet8_hooks hooks = {.malloc_fn = NULL,
                            .free_fn = NULL,
                            .printf_fn = &moronet8_printf_impl,
                            .host_session_fn = &moronet8_session_host_impl,
                            .join_session_fn = &moronet8_session_join_impl,
                            .delete_session_fn = &moronet8_session_delete_impl,
                            .broadcast_session_fn = &moronet8_session_broadcast_impl,
                            .receive_session_fn = &moronet8_session_receive_impl,
                            .session_state_get_fn = &moronet8_session_state_get_impl,
                            .poll_session_fn = &moronet8_session_poll_impl};
    moronet8_init_hooks(&hooks);

    moronet8 vm;
    moronet8_init(&vm);

    moronet8_cart cart;
    char cart_dir[256];

    char buf[256];
    fs_current_dir(buf, 256);

    moronet8_cart_load_dir(&cart, args.cart);
    moronet8_load_cart(&vm, &cart.data);
    dump(&cart, MORONET8_API_CART);

    fs_join_path(cart_dir, 256, buf, "bios");
    moronet8_cart_load_dir(&cart, cart_dir);
    moronet8_load_bios(&vm, &cart.data);
    dump(&cart, MORONET8_API_BIOS);

    return moronet8_run_player(&vm);
}
