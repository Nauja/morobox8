#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing memory.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// clang-format off
/* List of hooks. */
#define MOROBOX8_HOOKS_LIST(macro) \
    macro(malloc, void*, size_t size, size, VOID) \
    macro(free, void, void* ptr, ptr, VOID) \
    macro(printf, void, const char *fmt MOROBOX8_COMMA va_list args, fmt MOROBOX8_COMMA args, VOID)
    // clang-format on

#if MOROBOX8_HOOK

    struct morobox8_hooks
    {
#define MOROBOX8_HOOKS_DEF(name, rtype, params, ...) \
    rtype (*name##_fn)(params);
        MOROBOX8_HOOKS_LIST(MOROBOX8_HOOKS_DEF)
#undef MOROBOX8_HOOKS_DEF
    };

    /* Initialize hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_init_hooks(struct morobox8_hooks *hooks);

#endif

    MOROBOX8_PUBLIC(void *)
    morobox8_malloc(size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_free(void *p);

    MOROBOX8_PUBLIC(void)
    morobox8_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
