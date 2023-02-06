#pragma once
/**
 * Interface to be implemented by platform specific code
 * for managing networking sessions.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "network/session_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

// clang-format off
/* List of session hooks. */
#define MOROBOX8_SESSION_HOOKS_LIST(macro) \
    macro(host, struct morobox8_session *, const char *host, host, NULL) \
    macro(join, struct morobox8_session *, const char *host, host, NULL) \
    macro(free, void, struct morobox8_session *session, session, VOID) \
    macro(broadcast, void, struct morobox8_session *session MOROBOX8_COMMA const void *buf MOROBOX8_COMMA size_t size, session MOROBOX8_COMMA buf MOROBOX8_COMMA size, VOID) \
    macro(receive, size_t, struct morobox8_session *session MOROBOX8_COMMA void *buf MOROBOX8_COMMA size_t size, session MOROBOX8_COMMA buf MOROBOX8_COMMA size, ZERO) \
    macro(get_state, enum morobox8_session_state, struct morobox8_session *session, session, ZERO) \
    macro(poll, void, struct morobox8_session *session, session, VOID)
    // clang-format on

#if MOROBOX8_HOOK

    struct morobox8_session_hooks
    {
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params, ...) \
    rtype (*name##_fn)(params);
        MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
    };

    /* Initialize session hooks. */
    MOROBOX8_PUBLIC(void)
    morobox8_session_init_hooks(struct morobox8_session_hooks *hooks);

#endif

#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params, ...) \
    MOROBOX8_PUBLIC(rtype)                                   \
    morobox8_session_##name(params);
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF

#ifdef __cplusplus
}
#endif
