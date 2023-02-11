#pragma once

#include "argparse.h"

#ifdef __cplusplus
extern "C"
{
#endif

// clang-format off
#define MOROBOX8_CMD_LIST(macro) \
    macro(info, INFO) \
    macro(pack, PACK) \
    macro(run, RUN) \
    macro(unpack, UNPACK)
    // clang-format on

#define MOROBOX8_CMD_DEF(name, ...) \
    int morobox8_main_##name(int argc, char **argv);
    MOROBOX8_CMD_LIST(MOROBOX8_CMD_DEF)
#undef MOROBOX8_CMD_DEF

#define MOROBOX8_MAIN_OPT_PARAM_DEF(name, ctype, type, post, help) OPT_##type('\0', #name, &args.name, help, 0, 0, 0),
#define MOROBOX8_MAIN_C(name, NAME)                                        \
    int morobox8_main_##name(int argc, char **argv)                        \
    {                                                                      \
        morobox8_##name##_args args;                                       \
        memset(&args, 0, sizeof(morobox8_##name##_args));                  \
                                                                           \
        struct argparse_option options[] =                                 \
            {                                                              \
                OPT_HELP(),                                                \
                MOROBOX8_##NAME##_PARAMS_LIST(MOROBOX8_MAIN_OPT_PARAM_DEF) \
                    OPT_END(),                                             \
            };                                                             \
                                                                           \
        const char *const usage[] = {                                      \
            MOROBOX8_NAME " " #name " [options]",                          \
            NULL,                                                          \
        };                                                                 \
                                                                           \
        struct argparse argparse;                                          \
        argparse_init(&argparse, options, usage, 0);                       \
        argc = argparse_parse(&argparse, argc, (const char **)argv);       \
        if (argc < 1)                                                      \
        {                                                                  \
            argparse_usage(&argparse);                                     \
            return -1;                                                     \
        }                                                                  \
                                                                           \
        if (!morobox8_parse_##name(&args, argc, argv))                     \
        {                                                                  \
            argparse_usage(&argparse);                                     \
            return -1;                                                     \
        }                                                                  \
                                                                           \
        return morobox8_handle_##name(&args);                              \
    }

#ifdef __cplusplus
}
#endif
