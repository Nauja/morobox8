#include "network/packet.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

typedef struct moronet8_packet_reader moronet8_packet_reader;
typedef struct moronet8_packet_writer moronet8_packet_writer;

MORONET8_PUBLIC(moronet8_packet_reader *)
moronet8_packet_reader_create(const moronet8_u8 *buf, size_t size)
{
    moronet8_packet_reader *o = (moronet8_packet_reader *)malloc(sizeof(moronet8_packet_reader));
    if (!o)
    {
        return NULL;
    }

    moronet8_packet_reader_init(o, buf, size);
    return o;
}

MORONET8_PUBLIC(moronet8_packet_writer *)
moronet8_packet_writer_create(moronet8_u8 *buf, size_t size)
{
    moronet8_packet_writer *o = (moronet8_packet_writer *)malloc(sizeof(moronet8_packet_writer));
    if (!o)
    {
        return NULL;
    }

    moronet8_packet_writer_init(o, buf, size);
    return o;
}

MORONET8_PUBLIC(void)
moronet8_packet_reader_init(moronet8_packet_reader *reader, const moronet8_u8 *buf, size_t size)
{
    memset(reader, 0, sizeof(moronet8_packet_reader));
    reader->buf = buf;
    reader->size = size;
}

MORONET8_PUBLIC(void)
moronet8_packet_writer_init(moronet8_packet_writer *writer, moronet8_u8 *buf, size_t size)
{
    memset(writer, 0, sizeof(moronet8_packet_writer));
    writer->buf = buf;
    writer->size = size;
}

MORONET8_PUBLIC(moronet8_u8)
moronet8_packet_read_u8(moronet8_packet_reader *reader)
{
    if (reader->offset >= reader->size)
    {
        return 0;
    }

    reader->offset++;
    return reader->buf[reader->offset - 1];
}

MORONET8_PUBLIC(void)
moronet8_packet_write_u8(moronet8_packet_writer *writer, moronet8_u8 value)
{
    if (writer->offset < writer->size)
    {
        writer->buf[writer->offset] = value;
        writer->offset++;
    }
}

MORONET8_PUBLIC(moronet8_u16)
moronet8_packet_read_u16(moronet8_packet_reader *reader)
{
    return (((moronet8_u16)moronet8_packet_read_u8(reader)) << 8) + ((moronet8_u16)moronet8_packet_read_u8(reader));
}

MORONET8_PUBLIC(void)
moronet8_packet_write_u16(moronet8_packet_writer *writer, moronet8_u16 value)
{
    moronet8_packet_write_u8(writer, (moronet8_u8)((value & 0xFF00) >> 8));
    moronet8_packet_write_u8(writer, (moronet8_u8)(value & 0xFF));
}

MORONET8_PUBLIC(moronet8_u32)
moronet8_packet_read_u32(moronet8_packet_reader *reader)
{
    return (((moronet8_u32)moronet8_packet_read_u16(reader)) << 16) + ((moronet8_u32)moronet8_packet_read_u16(reader));
}

MORONET8_PUBLIC(void)
moronet8_packet_write_u32(moronet8_packet_writer *writer, moronet8_u32 value)
{
    moronet8_packet_write_u16(writer, (moronet8_u16)((value & 0xFFFF0000) >> 16));
    moronet8_packet_write_u16(writer, (moronet8_u16)(value & 0xFFFF));
}

MORONET8_PUBLIC(moronet8_s8)
moronet8_packet_read_s8(moronet8_packet_reader *reader)
{
    return (moronet8_s8)moronet8_packet_read_u8(reader);
}

MORONET8_PUBLIC(void)
moronet8_packet_write_s8(moronet8_packet_writer *writer, moronet8_s8 value)
{
    moronet8_packet_write_u8(writer, (moronet8_u8)value);
}

MORONET8_PUBLIC(moronet8_s16)
moronet8_packet_read_s16(moronet8_packet_reader *reader)
{
    return (moronet8_s16)moronet8_packet_read_u16(reader);
}

MORONET8_PUBLIC(void)
moronet8_packet_write_s16(moronet8_packet_writer *writer, moronet8_s16 value)
{
    moronet8_packet_write_u16(writer, (moronet8_u16)value);
}

MORONET8_PUBLIC(moronet8_s32)
moronet8_packet_read_s32(moronet8_packet_reader *reader)
{
    return (moronet8_s32)moronet8_packet_read_u32(reader);
}

MORONET8_PUBLIC(void)
moronet8_packet_write_s32(moronet8_packet_writer *writer, moronet8_s32 value)
{
    moronet8_packet_write_u32(writer, (moronet8_u32)value);
}

MORONET8_PUBLIC(size_t)
moronet8_packet_read_string(moronet8_packet_reader *reader, char *buf, size_t size)
{
    size_t len = moronet8_packet_read_u32(reader);
    for (size_t i = 0; i < len && i < size; ++i)
    {
        buf[i] = (char)moronet8_packet_read_u8(reader);
    }

    return len;
}

MORONET8_PUBLIC(size_t)
moronet8_packet_write_string(moronet8_packet_writer *writer, const char *buf, size_t size)
{
    size_t i = 0;
    moronet8_packet_write_u32(writer, size);
    for (i = 0; i < size; ++i)
    {
        moronet8_packet_write_u8(writer, (moronet8_u8)buf[i]);
    }

    return i;
}

MORONET8_PUBLIC(size_t)
moronet8_packet_reader_size(moronet8_packet_reader *reader)
{
    return reader->offset;
}

MORONET8_PUBLIC(size_t)
moronet8_packet_writer_size(moronet8_packet_writer *writer)
{
    return writer->offset;
}

MORONET8_PUBLIC(void)
moronet8_packet_reader_seek(moronet8_packet_reader *reader, size_t offset)
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

MORONET8_PUBLIC(void)
moronet8_packet_writer_seek(moronet8_packet_writer *writer, size_t offset)
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

MORONET8_PUBLIC(const moronet8_u8 *)
moronet8_packet_reader_buffer(struct moronet8_packet_reader *reader, size_t *size)
{
    *size = reader->size - reader->offset;
    return reader->buf + reader->offset;
}

MORONET8_PUBLIC(moronet8_u8 *)
moronet8_packet_writer_buffer(struct moronet8_packet_writer *writer, size_t *size)
{
    *size = writer->size - writer->offset;
    return writer->buf + writer->offset;
}

MORONET8_PUBLIC(void)
moronet8_packet_reader_delete(moronet8_packet_reader *reader)
{
    free(reader);
}

MORONET8_PUBLIC(void)
moronet8_packet_writer_delete(moronet8_packet_writer *writer)
{
    free(writer);
}
