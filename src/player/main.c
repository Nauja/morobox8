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

#if HAVE_STDIO_H
#include <stdio.h>
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

MOROBOX8_PUBLIC(void *)
morobox8_malloc(size_t size)
{
    return malloc(size);
}

MOROBOX8_PUBLIC(void)
morobox8_free(void *p)
{
    free(p);
}

MOROBOX8_PUBLIC(void)
morobox8_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

int main(int argc, char **argv)
{
    return morobox8_parse_args(argc, argv);
}
