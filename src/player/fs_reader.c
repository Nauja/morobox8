/**
 * Reader implementation using libfs.
 */
#include "morobox8_config.h"
#include "morobox8_defines.h"
#include "morobox8_limits.h"
#include "morobox8_types.h"
#include "cart/cart.h"

#include "fs.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

typedef struct morobox8_cart morobox8_cart;

typedef struct morobox8_reader
{
    morobox8_cart cart;
} morobox8_reader;

MOROBOX8_PUBLIC(morobox8_u32)
morobox8_reader_read(struct morobox8_reader *reader, morobox8_u32 address, void *buf, size_t size)
{
    address = min(address, sizeof(morobox8_cart));
    size_t len = min(size, sizeof(morobox8_cart) - address);
    memcpy(buf, &((morobox8_u8 *)&reader->cart)[address], len);
    return len;
}

MOROBOX8_PUBLIC(int)
morobox8_reader_read_u8(struct morobox8_reader *reader, morobox8_u32 address, morobox8_u8 *val)
{
    return morobox8_reader_read(reader, address, (void *)val, sizeof(morobox8_u8));
}

MOROBOX8_PUBLIC(int)
morobox8_reader_read_u32(struct morobox8_reader *reader, morobox8_u32 address, morobox8_u32 *val)
{
    return morobox8_reader_read(reader, address, (void *)val, sizeof(morobox8_u32));
}
