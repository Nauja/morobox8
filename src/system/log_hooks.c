#include "system/log_hooks.h"
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_hooks.h"

#include <time.h>
#include <stdio.h>

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};

MOROBOX8_PUBLIC(void)
log_log(int level, const char *file, int line, const char *fmt, ...)
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
    morobox8_printf("%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buf, level_colors[level], level_strings[level],
                    file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    morobox8_printf("\n");
}
