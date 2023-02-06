/**
 * Filesystem implementation using libfs.
 */
#include "morobox8_config.h"
#include "morobox8_limits.h"
#include "morobox8_types.h"

#include "fs.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_file morobox8_file;

MOROBOX8_PUBLIC(int)
morobox8_fs_exist(const char *name, size_t size)
{
    char buf[MOROBOX8_FILENAME_SIZE];
    snprintf(&buf[0], MOROBOX8_FILENAME_SIZE, "%.*s", size, name);
    return fs_exist(buf);
}

MOROBOX8_PUBLIC(morobox8_file *)
morobox8_fs_open(const char *name, size_t size, const char *mode)
{
    char buf[MOROBOX8_FILENAME_SIZE];
    snprintf(&buf[0], MOROBOX8_FILENAME_SIZE, "%.*s", size, name);
    return (morobox8_file *)fopen(buf, mode);
}

MOROBOX8_PUBLIC(void)
morobox8_fs_seek(morobox8_file *file, morobox8_u32 offset)
{
    fseek((FILE *)file, offset, SEEK_SET);
}

MOROBOX8_PUBLIC(void)
morobox8_fs_read(morobox8_file *file, void *buf, morobox8_u32 size)
{
    fread(buf, 1, size, (FILE *)file);
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_fs_read_byte(morobox8_file *file)
{
    char buf[1];
    fread(&buf[0], 1, 1, (FILE *)file);
    return (morobox8_u8)buf[0];
}

MOROBOX8_PUBLIC(void)
morobox8_fs_close(morobox8_file *file)
{
    fclose((FILE *)file);
}
