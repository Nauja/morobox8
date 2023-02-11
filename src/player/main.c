#include "player/main.h"
#include "morobox8.h"
#include "morobox8_hooks.h"
#include "system/storage_hooks.h"
#include "network/session_hooks.h"
#include "tool/packer.h"

#include "fs.h"
#include "argparse.h"

#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static const char *const usage[] = {
    MOROBOX8_NAME " command [options]",
    NULL,
};

typedef struct command_struct
{
    const char *cmd;
    int (*fn)(int, char **);
} command_struct;

#define MOROBOX8_CMD_DEF(name, NAME) \
    {#name, morobox8_main_##name},
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

int main(int argc, char **argv)
{
    return morobox8_parse_args(argc, argv);
}
