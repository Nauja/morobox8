#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing the external reader.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct morobox8_file;
    struct morobox8_reader;

// clang-format off
/* List of reader hooks. */
#define MOROBOX8_READER_HOOKS_LIST(macro) \
    macro(read, morobox8_u32, struct morobox8_reader* reader MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA void* buf MOROBOX8_COMMA morobox8_u32 size, reader MOROBOX8_COMMA address MOROBOX8_COMMA buf MOROBOX8_COMMA size, FALSE) \
    macro(read_u8, int, struct morobox8_reader* reader MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA morobox8_u8* val, reader MOROBOX8_COMMA address MOROBOX8_COMMA val, FALSE) \
    macro(read_u32, int, struct morobox8_reader* reader MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA morobox8_u32* val, reader MOROBOX8_COMMA address MOROBOX8_COMMA val, FALSE)
    // clang-format on

#if MOROBOX8_HOOK

    struct morobox8_reader_hooks
    {
#define MOROBOX8_READER_HOOKS_DEF(name, rtype, params, ...) \
    rtype (*name##_fn)(params);
        MOROBOX8_READER_HOOKS_LIST(MOROBOX8_READER_HOOKS_DEF)
#undef MOROBOX8_READER_HOOKS_DEF
    };

    /* Initialize reader hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_reader_init_hooks(struct morobox8_reader_hooks *hooks);

#endif

#define MOROBOX8_READER_HOOKS_DEF(name, rtype, params, ...) \
    MOROBOX8_PUBLIC(rtype)                                  \
    morobox8_reader_##name(params);
    MOROBOX8_READER_HOOKS_LIST(MOROBOX8_READER_HOOKS_DEF)
#undef MOROBOX8_READER_HOOKS_DEF

#ifdef __cplusplus
}
#endif
