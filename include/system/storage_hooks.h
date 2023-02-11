#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing the internal storage.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct morobox8_file;
    struct morobox8_storage;

// clang-format off
/* List of storage hooks. */
#define MOROBOX8_STORAGE_HOOKS_LIST(macro) \
    macro(exist, int, struct morobox8_storage *storage MOROBOX8_COMMA const char* name MOROBOX8_COMMA size_t size, storage MOROBOX8_COMMA name MOROBOX8_COMMA size, FALSE) \
    macro(open, struct morobox8_file*, struct morobox8_storage *storage MOROBOX8_COMMA const char* name MOROBOX8_COMMA size_t size MOROBOX8_COMMA const char* mode, storage MOROBOX8_COMMA name MOROBOX8_COMMA size MOROBOX8_COMMA mode, NULL) \
    macro(seek, int, struct morobox8_storage *storage MOROBOX8_COMMA struct morobox8_file* file MOROBOX8_COMMA morobox8_u32 offset, storage MOROBOX8_COMMA file MOROBOX8_COMMA offset, FALSE) \
    macro(read, morobox8_u32, struct morobox8_storage *storage MOROBOX8_COMMA struct morobox8_file* file MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA void* buf MOROBOX8_COMMA morobox8_u32 size, storage MOROBOX8_COMMA file MOROBOX8_COMMA address MOROBOX8_COMMA buf MOROBOX8_COMMA size, 0) \
    macro(read_u8, int, struct morobox8_storage *storage MOROBOX8_COMMA struct morobox8_file* file MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA morobox8_u8* val, storage MOROBOX8_COMMA file MOROBOX8_COMMA address MOROBOX8_COMMA val, FALSE) \
    macro(read_u32, int, struct morobox8_storage *storage MOROBOX8_COMMA struct morobox8_file* file MOROBOX8_COMMA morobox8_u32 address MOROBOX8_COMMA morobox8_u32* val, storage MOROBOX8_COMMA file MOROBOX8_COMMA address MOROBOX8_COMMA val, FALSE) \
    macro(close, void, struct morobox8_storage *storage MOROBOX8_COMMA struct morobox8_file* file, storage MOROBOX8_COMMA file, VOID)
    // clang-format on

#if MOROBOX8_HOOK

    struct morobox8_storage_hooks
    {
#define MOROBOX8_STORAGE_HOOKS_DEF(name, rtype, params, ...) \
    rtype (*name##_fn)(params);
        MOROBOX8_STORAGE_HOOKS_LIST(MOROBOX8_STORAGE_HOOKS_DEF)
#undef MOROBOX8_STORAGE_HOOKS_DEF
    };

    /* Initialize storage hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_storage_init_hooks(struct morobox8_storage_hooks *hooks);

#endif

#define MOROBOX8_STORAGE_HOOKS_DEF(name, rtype, params, ...) \
    MOROBOX8_PUBLIC(rtype)                                   \
    morobox8_storage_##name(params);
    MOROBOX8_STORAGE_HOOKS_LIST(MOROBOX8_STORAGE_HOOKS_DEF)
#undef MOROBOX8_STORAGE_HOOKS_DEF

#ifdef __cplusplus
}
#endif
