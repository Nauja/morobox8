#include "morobox8_config.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

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
