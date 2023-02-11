/**
 * Storage implementation using libfs.
 */
#include "player/fs_storage.h"
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_limits.h"
#include "morobox8_types.h"
#include "system/log_hooks.h"

#include "fs.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_file morobox8_file;
typedef struct morobox8_storage morobox8_storage;

MOROBOX8_PUBLIC(int)
morobox8_storage_exist(morobox8_storage *storage, const char *name, size_t size)
{
    char buf[MOROBOX8_FILENAME_SIZE];
    snprintf(&buf[0], MOROBOX8_FILENAME_SIZE, "%.*s", size, name);
    return fs_exist(buf);
}

MOROBOX8_PUBLIC(morobox8_file *)
morobox8_storage_open(morobox8_storage *storage, const char *name, size_t size, const char *mode)
{
    log_debug("Open file %.*s", size, name);
    if (!morobox8_storage_exist(storage, name, size))
    {
        log_warn("File %.*s not found", size, name);
        return NULL;
    }
    char buf[MOROBOX8_FILENAME_SIZE];
    snprintf(&buf[0], MOROBOX8_FILENAME_SIZE, "%.*s", size, name);
    return (morobox8_file *)fopen(buf, mode);
}

MOROBOX8_PUBLIC(int)
morobox8_storage_seek(morobox8_storage *storage, morobox8_file *file, morobox8_u32 offset)
{
    fseek((FILE *)file, offset, SEEK_SET);
    return MOROBOX8_TRUE;
}

MOROBOX8_PUBLIC(morobox8_u32)
morobox8_storage_read(morobox8_storage *storage, morobox8_file *file, morobox8_u32 address, void *buf, morobox8_u32 size)
{
    if (!morobox8_storage_seek(storage, file, address))
    {
        return 0;
    }

    return fread(buf, 1, size, (FILE *)file);
}

MOROBOX8_PUBLIC(int)
morobox8_storage_read_u8(morobox8_storage *storage, morobox8_file *file, morobox8_u32 address, morobox8_u8 *val)
{
    return morobox8_storage_read(storage, file, address, (void *)val, sizeof(morobox8_u8));
}

MOROBOX8_PUBLIC(int)
morobox8_storage_read_u32(morobox8_storage *storage, morobox8_file *file, morobox8_u32 address, morobox8_u8 *val)
{
    return morobox8_storage_read(storage, file, address, (void *)val, sizeof(morobox8_u32));
}

MOROBOX8_PUBLIC(void)
morobox8_storage_close(morobox8_storage *storage, morobox8_file *file)
{
    fclose((FILE *)file);
}
