#include "morobox8.h"
#include "filesystem/filesystem_hooks.h"
#include "network/session_hooks.h"
#include "pack.h"

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
typedef struct morobox8_fs_hooks morobox8_fs_hooks;
typedef struct morobox8_session_hooks morobox8_session_hooks;
typedef enum morobox8_session_state morobox8_session_state;
typedef struct morobox8_session morobox8_session;
typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8 morobox8;

int morobox8_run_player(morobox8 *vm);

static const char *const usage[] = {
    MOROBOX8_NAME " command [options]",
    NULL,
};

// clang-format off
#define MOROBOX8_PACK_PARAMS_LIST(macro)                                                     \
    macro(output,       char*,  STRING,     "",         "output file")                        \
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
    char *target;
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

static int parse_pack(morobox8_pack_args *args, int argc, char *argv[])
{
    if (argc == 1)
    {
        args->target = argv[0];
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

static int handle_pack(morobox8_pack_args *args)
{
    morobox8_cart cart;
    if (!morobox8_pack(args->target, &cart))
    {
        morobox8_printf("Pack failed\n");
        return MOROBOX8_FALSE;
    }

    char buf[MOROBOX8_FILENAME_SIZE];
    if (args->output)
    {
        memcpy(&buf[0], args->output, strlen(args->output));
    }
    else
    {
        strncat(&buf[0], args->target, strlen(args->target));
        strncat(&buf[0], ".mb8", 4);
    }
    printf("Dump cart to %s\n", buf);

    if (!fs_write_file(buf, (void *)&cart, sizeof(morobox8_cart)))
    {
        morobox8_printf("Can't write output\n");
        return 0;
    }

    return MOROBOX8_TRUE;
}

static int parse_unpack(morobox8_unpack_args *args, int argc, char *argv[])
{
    return MOROBOX8_TRUE;
}

static int handle_unpack(morobox8_unpack_args *args)
{
    return MOROBOX8_TRUE;
}

static int parse_run(morobox8_run_args *args, int argc, char *argv[])
{
    if (argc == 1)
    {
        args->cart = argv[0];
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

static int handle_run(morobox8_run_args *args)
{
    morobox8 vm;
    morobox8_init(&vm);

    morobox8_cart cart;
    if (fs_is_file(args->cart))
    {
        if (!morobox8_cart_load_file(&cart, args->cart))
        {
            return MOROBOX8_FALSE;
        }
    }
    else if (!morobox8_cart_load_dir(&cart, args->cart))
    {
        return MOROBOX8_FALSE;
    }

    morobox8_load_cart(&vm, &cart.data);

    morobox8_cart_load_dir(&cart, "bios");
    morobox8_load_bios(&vm, &cart.data);

    return morobox8_run_player(&vm);
}

#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) OPT_##type('\0', #name, &args.name, help, 0, 0, 0),
#define MOROBOX8_CMD_DEF(name, NAME)                                   \
    static int parse_command_##name(int argc, char **argv)             \
    {                                                                  \
        morobox8_##name##_args args;                                   \
        memset(&args, 0, sizeof(morobox8_##name##_args));              \
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
        if (!parse_##name(&args, argc, argv))                          \
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

static morobox8_hooks morobox8_hooks_impl = {.malloc_fn = NULL,
                                             .free_fn = NULL,
                                             .printf_fn = &morobox8_printf_impl};

#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    rtype morobox8_fs_##name##_impl(params);
MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF

static morobox8_fs_hooks morobox8_fs_hooks_impl = {
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    .name##_fn = morobox8_fs_##name##_impl,
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
};

#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    rtype morobox8_session_##name##_impl(params);
MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF

static morobox8_session_hooks morobox8_session_hooks_impl = {
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    .name##_fn = morobox8_session_##name##_impl,
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
};

int main(int argc, char **argv)
{
    morobox8_init_hooks(&morobox8_hooks_impl);
    morobox8_fs_init_hooks(&morobox8_fs_hooks_impl);
    morobox8_session_init_hooks(&morobox8_session_hooks_impl);

    return morobox8_parse_args(argc, argv);
}
