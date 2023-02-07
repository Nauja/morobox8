#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "tool/packer.h"

#include "fs.h"
#include "argparse.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8 morobox8;

// clang-format off
#define MOROBOX8_RUN_PARAMS_LIST(macro)                                                     \
    macro(join,         char*,  STRING,     "",         "host ip:port")                     \
    macro(scale,        int,    INTEGER,    "=<int>",   "main window scale")                \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
    char *cart;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_RUN_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_run_args;

static int morobox8_parse_run(morobox8_run_args *args, int argc, char *argv[])
{
    if (argc == 1)
    {
        args->cart = argv[0];
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

int morobox8_run_player(morobox8 *vm);

static int morobox8_handle_run(morobox8_run_args *args)
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

MOROBOX8_MAIN_C(run, RUN)
