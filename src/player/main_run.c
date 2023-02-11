#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "cart/cart.h"
#include "tool/packer.h"
#include "tool/unpacker.h"
#include "player/fs_storage.h"

#include "fs.h"
#include "argparse.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_cart morobox8_cart;
typedef struct morobox8_packer morobox8_packer;
typedef struct morobox8_reader morobox8_reader;
typedef struct morobox8_storage morobox8_storage;
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

    FILE *f = fopen(args->cart, "rb");
    vm.reader = (morobox8_reader *)f;

    morobox8_storage storage = {
        .root = ""};
    vm.storage = &storage;

    morobox8_reset(&vm);
    return morobox8_run_player(&vm);
}

MOROBOX8_MAIN_C(run, RUN)
