#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "tool/info.h"

#include "fs.h"
#include "argparse.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

// clang-format off
#define MOROBOX8_INFO_PARAMS_LIST(macro)                                                     \
    macro(v,            int,    BOOLEAN,    "",         "print program version")  \
    macro(vv,           int,    BOOLEAN,    "",         "print program version")  \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
    char *target;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_INFO_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_info_args;

static int morobox8_parse_info(morobox8_info_args *args, int argc, char *argv[])
{
    if (argc == 1)
    {
        args->target = argv[0];
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

static int morobox8_handle_info(morobox8_info_args *args)
{
    const char *c = strrchr(args->target, '.');
    if (!c || strncmp(c, ".mb8", 4) != 0)
    {
        morobox8_printf("%s is not a cart\n", args->target);
        return MOROBOX8_FALSE;
    }

    char buf[MOROBOX8_INFO_SIZE];
    size_t size;
    FILE *f = fopen(args->target, "rb");
    int result = morobox8_info_file(f, &buf[0], MOROBOX8_INFO_SIZE, &size, args->vv ? MOROBOX8_INFO_EXTRA_VERBOSE : MOROBOX8_INFO_VERBOSE);
    fclose(f);
    if (result)
    {
        morobox8_printf("%s", &buf[0]);
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

MOROBOX8_MAIN_C(info, INFO)
