#include "morobox8_hooks.h"
#include "morobox8_config.h"
#include "morobox8_defines.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if MOROBOX8_HOOK

typedef struct morobox8_hooks morobox8_hooks;

#ifndef MOROBOX8_MALLOC
#ifdef HAVE_MALLOC
/**
 * Defines the malloc function used by moro8 at compile time.
 *
 * @code
 * void* my_malloc(size_t size)
 * {
 *     // do something
 * }
 *
 * #define MOROBOX8_MALLOC my_malloc
 * @endcode
 */
#define MOROBOX8_MALLOC malloc
#else
#define MOROBOX8_MALLOC(size) NULL
#endif
#endif

#ifndef MOROBOX8_FREE
#ifdef HAVE_FREE
/**
 * Defines the free function used by moro8 at compile time.
 *
 * @code
 * void my_free(void* ptr)
 * {
 *     // do something
 * }
 *
 * #define MOROBOX8_FREE my_free
 * @endcode
 */
#define MOROBOX8_FREE free
#else
#define MOROBOX8_FREE(ptr)
#endif
#endif

#ifdef HAVE_MALLOC
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void *MOROBOX8_CDECL internal_malloc(size_t size)
{
    return MOROBOX8_MALLOC(size);
}

static void MOROBOX8_CDECL internal_free(void *pointer)
{
    MOROBOX8_FREE(pointer);
}
#else
#define internal_malloc MOROBOX8_MALLOC
#define internal_free MOROBOX8_FREE
#endif
#else
#define internal_malloc NULL
#define internal_free NULL
#endif

#ifdef HAVE_PRINTF
#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void MOROBOX8_CDECL internal_printf(const char *fmt, ...)
{
    printf(fmt, ...);
}
#else
#define internal_printf MOROBOX8_PRINTF
#endif
#else
#define internal_printf NULL
#endif

static morobox8_hooks morobox8_global_hooks = {
    internal_malloc,
    internal_free,
    internal_printf};

MOROBOX8_PUBLIC(void)
morobox8_init_hooks(morobox8_hooks *hooks)
{
#define MOROBOX8_HOOKS_DEF(name, rtype, params, ...) \
    if (hooks->name##_fn)                            \
        morobox8_global_hooks.name##_fn = hooks->name##_fn;
    MOROBOX8_HOOKS_LIST(MOROBOX8_HOOKS_DEF)
#undef MOROBOX8_HOOKS_DEF
}

MOROBOX8_PUBLIC(void *)
morobox8_malloc(size_t size)
{
    if (!morobox8_global_hooks.malloc_fn)
    {
        return NULL;
    }

    return morobox8_global_hooks.malloc_fn(size);
}

MOROBOX8_PUBLIC(void)
morobox8_free(void *p)
{
    if (morobox8_global_hooks.free_fn)
    {
        morobox8_global_hooks.free_fn(p);
    }
}

MOROBOX8_PUBLIC(void)
morobox8_printf(const char *fmt, ...)
{
    if (morobox8_global_hooks.printf_fn)
    {
        va_list args;
        va_start(args, fmt);
        morobox8_global_hooks.printf_fn(fmt, args);
        va_end(args);
    }
}

#endif
