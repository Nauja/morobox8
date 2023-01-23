#include "network/masterserver.h"
#include "network/relayserver.h"

#include "argparse.h"
#include "libwebsockets.h"

#include <stdio.h>
#include <signal.h>

typedef struct lws lws;
typedef struct lws_vhost lws_vhost;
typedef struct lws_context lws_context;
typedef struct lws_protocols lws_protocols;
typedef struct lws_context_creation_info lws_context_creation_info;
typedef struct lws_client_connect_info lws_client_connect_info;

#define SERVEREMU_NAME "serveremu"

// clang-format off
#define SERVEREMU_CMD_PARAMS_LIST(macro)                                                     \
    macro(master_port,  int,    INTEGER,    "=<int>",   "master port to listen to")                \
    macro(relay_port,   int,    INTEGER,    "=<int>",   "relay port to listen to")                \
    macro(version,      int,    BOOLEAN,    "",         "print program version")
// clang-format on

typedef struct
{
#define SERVEREMU_CMD_PARAMS_DEF(name, ctype, type, post, help) ctype name;
    SERVEREMU_CMD_PARAMS_LIST(SERVEREMU_CMD_PARAMS_DEF)
#undef SERVEREMU_CMD_PARAMS_DEF
} serveremu_args;

static serveremu_args serveremu_parse_args(int argc, char **argv)
{
    static const char *const usage[] =
        {
            SERVEREMU_NAME " [options]",
            NULL,
        };

    serveremu_args args = {.master_port = 0, .relay_port = 0};

    struct argparse_option options[] =
        {
            OPT_HELP(),
#define SERVEREMU_CMD_PARAMS_DEF(name, ctype, type, post, help) OPT_##type('\0', #name, &args.name, help, NULL, 0, 0),
            SERVEREMU_CMD_PARAMS_LIST(SERVEREMU_CMD_PARAMS_DEF)
#undef SERVEREMU_CMD_PARAMS_DEF
                OPT_END(),
        };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\n" SERVEREMU_NAME " startup options:", NULL);
    argc = argparse_parse(&argparse, argc, (const char **)argv);

    return args;
}

static int interrupted;

static lws_context *serveremu_create(const serveremu_args *args)
{
    lws_context_creation_info info;
    memset(&info, 0, sizeof(lws_context_creation_info));
    info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

    lws_context *ctxt = lws_create_context(&info);
    if (!ctxt)
    {
        fprintf(stderr, "server creation failed\n");
        return NULL;
    }

    if (!masterserver_create(ctxt, args->master_port) ||
        !relayserver_create(ctxt, args->relay_port))
    {
        lws_context_destroy(ctxt);
        return NULL;
    }

    printf("server created\n");
    return ctxt;
}

static void serveremu_sigint_handler(int sig)
{
    interrupted = 1;
}

int main(int argc, char **argv)
{
    serveremu_args args = serveremu_parse_args(argc, argv);

    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);
    lws_context *ctxt = serveremu_create(&args);
    if (!ctxt)
    {
        return 1;
    }

    signal(SIGINT, serveremu_sigint_handler);

    int n = 0;
    while (n >= 0 && !interrupted)
    {
        n = lws_service(ctxt, 0);
    }

    lws_context_destroy(ctxt);
    printf("server stopped\n");
    return 0;
}
