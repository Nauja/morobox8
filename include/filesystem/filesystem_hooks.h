#pragma once
/**
 * Interface for be implemented by platform specific code
 * for managing filesystem.
 */
#include "morobox8_config.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MOROBOX8_COMMA
#define MOROBOX8_COMMA ,
#endif

// clang-format off
/* List of filesystem hooks. */
#define MOROBOX8_FS_HOOKS_LIST(macro) \
    macro(exist, int, const char* name MOROBOX8_COMMA size_t size) \
    macro(open_bios, void, void) \
    macro(seek_bios, void, morobox8_u32 offset) \
    macro(read_bios, void, void* buf MOROBOX8_COMMA morobox8_u32 size) \
    macro(read_bios_byte, morobox8_u8, void) \
    macro(close_bios, void, void) \
    macro(open_cart, int, const char* name MOROBOX8_COMMA size_t size) \
    macro(read_cart, void, void* buf MOROBOX8_COMMA morobox8_u32 size) \
    macro(read_cart_byte, morobox8_u8, void) \
    macro(close_cart, void, void)
    // clang-format on

    struct morobox8_fs_hooks
    {
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    rtype (*name##_fn)(params);
        MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
    };

    /* Initialize filesystem hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_fs_init_hooks(struct morobox8_fs_hooks *hooks);

#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    MOROBOX8_PUBLIC(rtype)                         \
    morobox8_fs_##name(params);
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF

#ifdef __cplusplus
}
#endif
