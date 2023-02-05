#pragma once
/**
 * Interface for be implemented by platform specific code
 * for managing networking sessions.
 */
#include "morobox8_config.h"
#include "network/session_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MOROBOX8_COMMA
#define MOROBOX8_COMMA ,
#endif

// clang-format off
/* List of session hooks. */
#define MOROBOX8_SESSION_HOOKS_LIST(macro) \
    macro(host, struct morobox8_session *, const char *host) \
    macro(join, struct morobox8_session *, const char *host) \
    macro(free, void, struct morobox8_session *session) \
    macro(broadcast, void, struct morobox8_session *session MOROBOX8_COMMA const void *buf MOROBOX8_COMMA size_t size) \
    macro(receive, size_t, struct morobox8_session *session MOROBOX8_COMMA void *buf MOROBOX8_COMMA size_t size) \
    macro(get_state, enum morobox8_session_state, struct morobox8_session *session) \
    macro(poll, void, struct morobox8_session *session)
    // clang-format on

    struct morobox8_session_hooks
    {
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    rtype (*name##_fn)(params);
        MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
    };

    /* Initialize session hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_session_init_hooks(struct morobox8_session_hooks *hooks);

#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    MOROBOX8_PUBLIC(rtype)                              \
    morobox8_session_##name(params);
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF

#ifdef __cplusplus
}
#endif
