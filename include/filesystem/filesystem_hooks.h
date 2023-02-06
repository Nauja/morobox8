#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing filesystem.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct morobox8_file;

// clang-format off
/* List of filesystem hooks. */
#define MOROBOX8_FS_HOOKS_LIST(macro) \
    macro(exist, int, const char* name MOROBOX8_COMMA size_t size, name MOROBOX8_COMMA size, FALSE) \
    macro(open, struct morobox8_file*, const char* name MOROBOX8_COMMA size_t size MOROBOX8_COMMA const char* mode, name MOROBOX8_COMMA size MOROBOX8_COMMA mode, NULL) \
    macro(seek, void, struct morobox8_file* file MOROBOX8_COMMA morobox8_u32 offset, file MOROBOX8_COMMA offset, VOID) \
    macro(read, void, struct morobox8_file* file MOROBOX8_COMMA void* buf MOROBOX8_COMMA morobox8_u32 size, file MOROBOX8_COMMA buf MOROBOX8_COMMA size, VOID) \
    macro(read_byte, morobox8_u8, struct morobox8_file* file, file, ZERO) \
    macro(close, void, struct morobox8_file* file, file, VOID)
    // clang-format on

#if MOROBOX8_HOOK

    struct morobox8_fs_hooks
    {
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params, ...) \
    rtype (*name##_fn)(params);
        MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
    };

    /* Initialize filesystem hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_fs_init_hooks(struct morobox8_fs_hooks *hooks);

#endif

#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params, ...) \
    MOROBOX8_PUBLIC(rtype)                              \
    morobox8_fs_##name(params);
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF

#ifdef __cplusplus
}
#endif
