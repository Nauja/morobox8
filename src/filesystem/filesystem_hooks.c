#include "filesystem/filesystem_hooks.h"
#include "morobox8_defines.h"

typedef struct morobox8_fs_hooks morobox8_fs_hooks;

static morobox8_fs_hooks morobox8_fs_global_hooks = {
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    .name##_fn = NULL,
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
};

MOROBOX8_PUBLIC(void)
morobox8_fs_init_hooks(morobox8_fs_hooks *hooks)
{
#define MOROBOX8_FS_HOOKS_DEF(name, rtype, params) \
    if (hooks->name##_fn)                          \
        morobox8_fs_global_hooks.name##_fn = hooks->name##_fn;
    MOROBOX8_FS_HOOKS_LIST(MOROBOX8_FS_HOOKS_DEF)
#undef MOROBOX8_FS_HOOKS_DEF
}

MOROBOX8_PUBLIC(int)
morobox8_fs_exist(const char *name, size_t size)
{
    return MOROBOX8_FALSE;
}
