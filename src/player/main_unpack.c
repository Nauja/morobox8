#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "tool/packer.h"

#include "fs.h"
#include "argparse.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

// clang-format off
#define MOROBOX8_UNPACK_PARAMS_LIST(macro)                                                     \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
    char *cart;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_UNPACK_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_unpack_args;

static int morobox8_parse_unpack(morobox8_unpack_args *args, int argc, char *argv[])
{
    return MOROBOX8_TRUE;
}

static int morobox8_handle_unpack(morobox8_unpack_args *args)
{
    return MOROBOX8_TRUE;
}

MOROBOX8_MAIN_C(unpack, UNPACK)
