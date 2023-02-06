#include "network/session_hooks.h"
#include "morobox8_config.h"

#if MOROBOX8_HOOK

typedef struct morobox8_session_hooks morobox8_session_hooks;
typedef struct morobox8_session morobox8_session;

static morobox8_session_hooks morobox8_session_global_hooks = {
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params, ...) \
    .name##_fn = NULL,
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
};

MOROBOX8_PUBLIC(void)
morobox8_session_init_hooks(morobox8_session_hooks *hooks)
{
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params, ...) \
    if (hooks->name##_fn)                                    \
        morobox8_session_global_hooks.name##_fn = hooks->name##_fn;
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
}

#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params, args, rval)               \
    MOROBOX8_PUBLIC(rtype)                                                        \
    morobox8_session_##name(params)                                               \
    {                                                                             \
        if (!morobox8_session_global_hooks.name##_fn)                             \
        {                                                                         \
            MOROBOX8_RET##rval##_DEFAULT(void)                                    \
        }                                                                         \
                                                                                  \
        MOROBOX8_RET##rval##_VALUE(morobox8_session_global_hooks.name##_fn(args)) \
    }
MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF

#endif
