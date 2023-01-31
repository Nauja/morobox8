#include "morobox8.h"
#include "fs.h"
#include "argparse.h"

#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_hooks morobox8_hooks;
typedef enum morobox8_session_state morobox8_session_state;
typedef struct morobox8_session morobox8_session;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8 morobox8;

static const char *const usage[] = {
    MOROBOX8_NAME " command [options]",
    NULL,
};

// clang-format off
#define MOROBOX8_PACK_PARAMS_LIST(macro)                                                     \
    macro(version,      int,    BOOLEAN,    "",         "print program version")

#define MOROBOX8_UNPACK_PARAMS_LIST(macro)                                                     \
    macro(version,      int,    BOOLEAN,    "",         "print program version")

#define MOROBOX8_RUN_PARAMS_LIST(macro)                                                     \
    macro(join,         char*,  STRING,     "",         "host ip:port")                     \
    macro(scale,        int,    INTEGER,    "=<int>",   "main window scale")                \
    macro(version,      int,    BOOLEAN,    "",         "print program version")

#define MOROBOX8_CMD_LIST(macro) \
    macro(pack, PACK) \
    macro(unpack, UNPACK) \
    macro(run, RUN)
// clang-format on

typedef struct
{
    char *cart;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_PACK_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_pack_args;

typedef struct
{
    char *cart;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_UNPACK_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_unpack_args;

typedef struct
{
    char *cart;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_RUN_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_run_args;

static int handle_pack(morobox8_pack_args *args)
{
    return 0;
}

static int handle_unpack(morobox8_unpack_args *args)
{
    return 0;
}

static int handle_run(morobox8_run_args *args)
{
    return 0;
}

#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) OPT_##type('\0', #name, &args.name, help, 0, 0, 0),
#define MOROBOX8_CMD_DEF(name, NAME)                                   \
    static int parse_command_##name(int argc, char **argv)             \
    {                                                                  \
        morobox8_##name##_args args;                                   \
        (void)(args);                                                  \
                                                                       \
        struct argparse_option options[] =                             \
            {                                                          \
                OPT_HELP(),                                            \
                MOROBOX8_##NAME##_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF) \
                    OPT_END(),                                         \
            };                                                         \
                                                                       \
        const char *const usage[] = {                                  \
            MOROBOX8_NAME " " #name " [options]",                      \
            NULL,                                                      \
        };                                                             \
                                                                       \
        struct argparse argparse;                                      \
        argparse_init(&argparse, options, usage, 0);                   \
        argc = argparse_parse(&argparse, argc, (const char **)argv);   \
        if (argc < 1)                                                  \
        {                                                              \
            argparse_usage(&argparse);                                 \
            return -1;                                                 \
        }                                                              \
                                                                       \
        return handle_##name(&args);                                   \
    }
MOROBOX8_CMD_LIST(MOROBOX8_CMD_DEF)
#undef MOROBOX8_CMD_DEF
#undef MOROBOX8_CMD_PARAMS_DEF

typedef struct command_struct
{
    const char *cmd;
    int (*fn)(int, char **);
} command_struct;

#define MOROBOX8_CMD_DEF(name, NAME) \
    {#name, parse_command_##name},
static command_struct commands[] = {
    MOROBOX8_CMD_LIST(MOROBOX8_CMD_DEF){NULL, NULL}};
#undef MOROBOX8_CMD_DEF

static int morobox8_parse_args(int argc, char **argv)
{
    struct argparse_option options[] =
        {
            OPT_HELP(),
            OPT_END(),
        };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argc = argparse_parse(&argparse, argc, (const char **)argv);
    if (argc < 1)
    {
        argparse_usage(&argparse);
        return -1;
    }

    command_struct *cmd = &commands[0];
    while (cmd->cmd)
    {
        if (!strcmp(cmd->cmd, argv[0]))
        {
            return cmd->fn(argc, argv);
        }

        cmd++;
    }

    argparse_usage(&argparse);
    return -1;
}

int morobox8_run_player(morobox8 *vm);
morobox8_session *morobox8_session_host_impl(const char *host);
morobox8_session *morobox8_session_join_impl(const char *host);
void morobox8_session_delete_impl(morobox8_session *session);
void morobox8_session_broadcast_impl(morobox8_session *session, const void *buf, size_t size);
size_t morobox8_session_receive_impl(morobox8_session *session, void *buf, size_t size);
morobox8_session_state morobox8_session_state_get_impl(morobox8_session *session);
void morobox8_session_poll_impl(morobox8_session *session);

/*static void dump(morobox8_cart *cart, enum morobox8_api_type type)
{
    if (type == MOROBOX8_API_CART)
    {
        printf("dump cart\n");
    }
    else
    {
        printf("dump bios\n");
    }
    printf("cart size %d\n", sizeof(struct morobox8_cart_data));
    printf("cart num col %d\n", cart->data.num_colors);
    printf("code size %d\n", sizeof(struct morobox8_cart_code));
    memcpy(&cart->header.magic_code[0], (const void *)"MB8\n", 4);
    FILE *f = fopen(type == MOROBOX8_API_CART ? "cart.txt" : "bios.txt", "wb+");
    fwrite((void *)cart, 1, sizeof(struct morobox8_cart), f);
    fclose(f);
}*/

static void morobox8_printf_impl(const char *fmt, va_list args)
{
    vfprintf(stdout, fmt, args);
}

int main(int argc, char **argv)
{
    morobox8_hooks hooks = {.malloc_fn = NULL,
                            .free_fn = NULL,
                            .printf_fn = &morobox8_printf_impl,
                            .host_session_fn = &morobox8_session_host_impl,
                            .join_session_fn = &morobox8_session_join_impl,
                            .delete_session_fn = &morobox8_session_delete_impl,
                            .broadcast_session_fn = &morobox8_session_broadcast_impl,
                            .receive_session_fn = &morobox8_session_receive_impl,
                            .session_state_get_fn = &morobox8_session_state_get_impl,
                            .poll_session_fn = &morobox8_session_poll_impl};
    morobox8_init_hooks(&hooks);

    return morobox8_parse_args(argc, argv);
    /*
    morobox8 vm;
    morobox8_init(&vm);

    morobox8_cart cart;
    char cart_dir[256];

    char buf[256];
    fs_current_dir(buf, 256);

    morobox8_cart_load_dir(&cart, args.cart);
    morobox8_load_cart(&vm, &cart.data);
    dump(&cart, MOROBOX8_API_CART);

    fs_join_path(cart_dir, 256, buf, "bios");
    morobox8_cart_load_dir(&cart, cart_dir);
    morobox8_load_bios(&vm, &cart.data);
    dump(&cart, MOROBOX8_API_BIOS);

    return morobox8_run_player(&vm);*/
}
