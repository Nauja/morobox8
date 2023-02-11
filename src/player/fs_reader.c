/**
 * Reader implementation using the filesystem.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_limits.h"
#include "morobox8_types.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_reader morobox8_reader;

MOROBOX8_PUBLIC(morobox8_u32)
morobox8_reader_read(morobox8_reader *reader, morobox8_u32 address, void *buf, size_t size)
{
    FILE *f = (FILE *)reader;
    fseek(f, address, SEEK_SET);
    return fread(buf, 1, size, f);
}

MOROBOX8_PUBLIC(int)
morobox8_reader_read_u8(morobox8_reader *reader, morobox8_u32 address, morobox8_u8 *val)
{
    return morobox8_reader_read(reader, address, (void *)val, sizeof(morobox8_u8));
}

MOROBOX8_PUBLIC(int)
morobox8_reader_read_u32(morobox8_reader *reader, morobox8_u32 address, morobox8_u32 *val)
{
    return morobox8_reader_read(reader, address, (void *)val, sizeof(morobox8_u32));
}
