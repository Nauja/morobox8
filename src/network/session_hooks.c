#include "network/session_hooks.h"

typedef struct morobox8_session_hooks morobox8_session_hooks;
typedef struct morobox8_session morobox8_session;

static morobox8_session_hooks morobox8_session_global_hooks = {
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    .name##_fn = NULL,
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
};

MOROBOX8_PUBLIC(void)
morobox8_session_init_hooks(morobox8_session_hooks *hooks)
{
#define MOROBOX8_SESSION_HOOKS_DEF(name, rtype, params) \
    if (hooks->name##_fn)                               \
        morobox8_session_global_hooks.name##_fn = hooks->name##_fn;
    MOROBOX8_SESSION_HOOKS_LIST(MOROBOX8_SESSION_HOOKS_DEF)
#undef MOROBOX8_SESSION_HOOKS_DEF
}

MOROBOX8_PUBLIC(morobox8_session *)
morobox8_session_host(const char *host)
{
    if (!morobox8_session_global_hooks.host_fn)
    {
        return NULL;
    }

    return morobox8_session_global_hooks.host_fn(host);
}

MOROBOX8_PUBLIC(morobox8_session *)
morobox8_session_join(const char *host)
{
    if (!morobox8_session_global_hooks.join_fn)
    {
        return NULL;
    }

    return morobox8_session_global_hooks.join_fn(host);
}

MOROBOX8_PUBLIC(void)
morobox8_session_free(morobox8_session *session)
{
    if (!morobox8_session_global_hooks.free_fn)
    {
        return;
    }

    morobox8_session_global_hooks.free_fn(session);
}

MOROBOX8_PUBLIC(void)
morobox8_session_broadcast(morobox8_session *session, const void *buf, size_t size)
{
    if (!morobox8_session_global_hooks.broadcast_fn)
    {
        return;
    }

    morobox8_session_global_hooks.broadcast_fn(session, buf, size);
}

MOROBOX8_PUBLIC(size_t)
morobox8_session_receive(morobox8_session *session, void *buf, size_t size)
{
    if (!morobox8_session_global_hooks.receive_fn)
    {
        return 0;
    }

    return morobox8_session_global_hooks.receive_fn(session, buf, size);
}

MOROBOX8_PUBLIC(enum morobox8_session_state)
morobox8_session_get_state(morobox8_session *session)
{
    if (!session || !morobox8_session_global_hooks.get_state_fn)
    {
        return MOROBOX8_SESSION_CLOSED;
    }

    return morobox8_session_global_hooks.get_state_fn(session);
}

MOROBOX8_PUBLIC(void)
morobox8_session_poll(morobox8_session *session)
{
    if (!morobox8_session_global_hooks.poll_fn)
    {
        return;
    }

    morobox8_session_global_hooks.poll_fn(session);
}
