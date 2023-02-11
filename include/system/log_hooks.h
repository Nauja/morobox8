#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing the logs.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        LOG_TRACE,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL
    };

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

    MOROBOX8_PUBLIC(void)
    log_log(int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
