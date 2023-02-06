#include "filesystem/filesystem_hooks.h"
#include "morobox8_config.h"
#include "morobox8_defines.h"

#if MOROBOX8_HOOK

typedef struct morobox8_fs_hooks morobox8_fs_hooks;

static morobox8_fs_hooks morobox8_fs_global_hooks = {
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params, ...) \
    .name##_fn = NULL,
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
};

MOROBOX8_PUBLIC(void)
morobox8_fs_init_hooks(morobox8_fs_hooks *hooks)
{
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params, ...) \
    if (hooks->name##_fn)                               \
        morobox8_fs_global_hooks.name##_fn = hooks->name##_fn;
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
}

#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params, args, rval)               \
    MOROBOX8_PUBLIC(rtype)                                                   \
    morobox8_fs_##name(params)                                               \
    {                                                                        \
        if (!morobox8_fs_global_hooks.name##_fn)                             \
        {                                                                    \
            MOROBOX8_RET##rval##_DEFAULT(void)                               \
        }                                                                    \
                                                                             \
        MOROBOX8_RET##rval##_VALUE(morobox8_fs_global_hooks.name##_fn(args)) \
    }
MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF

#endif
