#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "tool/packer.h"

#include "fs.h"
#include "argparse.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_packer morobox8_packer;

// clang-format off
#define MOROBOX8_PACK_PARAMS_LIST(macro)                                                     \
    macro(output,       char*,  STRING,     "",         "output file")                        \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
    char *target;
#define MOROBOX8_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    MOROBOX8_PACK_PARAMS_LIST(MOROBOX8_CMD_PARAMS_DEF)
#undef MOROBOX8_CMD_PARAMS_DEF
} morobox8_pack_args;

static int morobox8_parse_pack(morobox8_pack_args *args, int argc, char *argv[])
{
    if (argc == 1)
    {
        args->target = argv[0];
        return MOROBOX8_TRUE;
    }

    return MOROBOX8_FALSE;
}

static int morobox8_handle_pack(morobox8_pack_args *args)
{
    char buf[MOROBOX8_FILENAME_SIZE];
    memset(&buf[0], 0, MOROBOX8_FILENAME_SIZE);

    if (args->output)
    {
        memcpy(&buf[0], args->output, strlen(args->output));
    }
    else
    {
        strncat(&buf[0], args->target, strlen(args->target));
        strncat(&buf[0], ".mb8", 4);
    }
    printf("Write cart to %s\n", buf);

    FILE *f = fopen(buf, "w+");
    if (!f)
    {
        morobox8_printf("Can't write file\n");
        return MOROBOX8_FALSE;
    }

    morobox8_packer *packer = morobox8_file_packer(f);
    morobox8_packer_add_dir(packer, args->target);
    morobox8_packer_pack(packer);
    morobox8_packer_delete(packer);
    return MOROBOX8_TRUE;
}

MOROBOX8_MAIN_C(pack, PACK)
