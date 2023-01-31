#include "network/packet.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

typedef struct morobox8_packet_reader morobox8_packet_reader;
typedef struct morobox8_packet_writer morobox8_packet_writer;

MOROBOX8_PUBLIC(morobox8_packet_reader *)
morobox8_packet_reader_create(const morobox8_u8 *buf, size_t size)
{
    morobox8_packet_reader *o = (morobox8_packet_reader *)malloc(sizeof(morobox8_packet_reader));
    if (!o)
    {
        return NULL;
    }

    morobox8_packet_reader_init(o, buf, size);
    return o;
}

MOROBOX8_PUBLIC(morobox8_packet_writer *)
morobox8_packet_writer_create(morobox8_u8 *buf, size_t size)
{
    morobox8_packet_writer *o = (morobox8_packet_writer *)malloc(sizeof(morobox8_packet_writer));
    if (!o)
    {
        return NULL;
    }

    morobox8_packet_writer_init(o, buf, size);
    return o;
}

MOROBOX8_PUBLIC(void)
morobox8_packet_reader_init(morobox8_packet_reader *reader, const morobox8_u8 *buf, size_t size)
{
    memset(reader, 0, sizeof(morobox8_packet_reader));
    reader->buf = buf;
    reader->size = size;
}

MOROBOX8_PUBLIC(void)
morobox8_packet_writer_init(morobox8_packet_writer *writer, morobox8_u8 *buf, size_t size)
{
    memset(writer, 0, sizeof(morobox8_packet_writer));
    writer->buf = buf;
    writer->size = size;
}

MOROBOX8_PUBLIC(morobox8_u8)
morobox8_packet_read_u8(morobox8_packet_reader *reader)
{
    if (reader->offset >= reader->size)
    {
        return 0;
    }

    reader->offset++;
    return reader->buf[reader->offset - 1];
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_u8(morobox8_packet_writer *writer, morobox8_u8 value)
{
    if (writer->offset < writer->size)
    {
        writer->buf[writer->offset] = value;
        writer->offset++;
    }
}

MOROBOX8_PUBLIC(morobox8_u16)
morobox8_packet_read_u16(morobox8_packet_reader *reader)
{
    return (((morobox8_u16)morobox8_packet_read_u8(reader)) << 8) + ((morobox8_u16)morobox8_packet_read_u8(reader));
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_u16(morobox8_packet_writer *writer, morobox8_u16 value)
{
    morobox8_packet_write_u8(writer, (morobox8_u8)((value & 0xFF00) >> 8));
    morobox8_packet_write_u8(writer, (morobox8_u8)(value & 0xFF));
}

MOROBOX8_PUBLIC(morobox8_u32)
morobox8_packet_read_u32(morobox8_packet_reader *reader)
{
    return (((morobox8_u32)morobox8_packet_read_u16(reader)) << 16) + ((morobox8_u32)morobox8_packet_read_u16(reader));
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_u32(morobox8_packet_writer *writer, morobox8_u32 value)
{
    morobox8_packet_write_u16(writer, (morobox8_u16)((value & 0xFFFF0000) >> 16));
    morobox8_packet_write_u16(writer, (morobox8_u16)(value & 0xFFFF));
}

MOROBOX8_PUBLIC(morobox8_s8)
morobox8_packet_read_s8(morobox8_packet_reader *reader)
{
    return (morobox8_s8)morobox8_packet_read_u8(reader);
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_s8(morobox8_packet_writer *writer, morobox8_s8 value)
{
    morobox8_packet_write_u8(writer, (morobox8_u8)value);
}

MOROBOX8_PUBLIC(morobox8_s16)
morobox8_packet_read_s16(morobox8_packet_reader *reader)
{
    return (morobox8_s16)morobox8_packet_read_u16(reader);
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_s16(morobox8_packet_writer *writer, morobox8_s16 value)
{
    morobox8_packet_write_u16(writer, (morobox8_u16)value);
}

MOROBOX8_PUBLIC(morobox8_s32)
morobox8_packet_read_s32(morobox8_packet_reader *reader)
{
    return (morobox8_s32)morobox8_packet_read_u32(reader);
}

MOROBOX8_PUBLIC(void)
morobox8_packet_write_s32(morobox8_packet_writer *writer, morobox8_s32 value)
{
    morobox8_packet_write_u32(writer, (morobox8_u32)value);
}

MOROBOX8_PUBLIC(size_t)
morobox8_packet_read_string(morobox8_packet_reader *reader, char *buf, size_t size)
{
    size_t len = morobox8_packet_read_u32(reader);
    for (size_t i = 0; i < len && i < size; ++i)
    {
        buf[i] = (char)morobox8_packet_read_u8(reader);
    }

    return len;
}

MOROBOX8_PUBLIC(size_t)
morobox8_packet_write_string(morobox8_packet_writer *writer, const char *buf, size_t size)
{
    size_t i = 0;
    morobox8_packet_write_u32(writer, size);
    for (i = 0; i < size; ++i)
    {
        morobox8_packet_write_u8(writer, (morobox8_u8)buf[i]);
    }

    return i;
}

MOROBOX8_PUBLIC(size_t)
morobox8_packet_reader_size(morobox8_packet_reader *reader)
{
    return reader->offset;
}

MOROBOX8_PUBLIC(size_t)
morobox8_packet_writer_size(morobox8_packet_writer *writer)
{
    return writer->offset;
}

MOROBOX8_PUBLIC(void)
morobox8_packet_reader_seek(morobox8_packet_reader *reader, size_t offset)
{
    if (offset < reader->size)
    {
        reader->offset = offset;
    }
    else
    {
        reader->offset = reader->size;
    }
}

MOROBOX8_PUBLIC(void)
morobox8_packet_writer_seek(morobox8_packet_writer *writer, size_t offset)
{
    if (offset < writer->size)
    {
        writer->offset = offset;
    }
    else
    {
        writer->offset = writer->size;
    }
}

MOROBOX8_PUBLIC(const morobox8_u8 *)
morobox8_packet_reader_buffer(struct morobox8_packet_reader *reader, size_t *size)
{
    *size = reader->size - reader->offset;
    return reader->buf + reader->offset;
}

MOROBOX8_PUBLIC(morobox8_u8 *)
morobox8_packet_writer_buffer(struct morobox8_packet_writer *writer, size_t *size)
{
    *size = writer->size - writer->offset;
    return writer->buf + writer->offset;
}

MOROBOX8_PUBLIC(void)
morobox8_packet_reader_delete(morobox8_packet_reader *reader)
{
    free(reader);
}

MOROBOX8_PUBLIC(void)
morobox8_packet_writer_delete(morobox8_packet_writer *writer)
{
    free(writer);
}
