#pragma once

#include "morobox8_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    MOROBOX8_PUBLIC(int)
    morobox8_fs_read_cart_chunk(const char *name, size_t size, void *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif
