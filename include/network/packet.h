#pragma once

#include "moronet8_config.h"
#include "moronet8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum moronet8_packet_type
    {
        MORONET8_PACKET_CREATE_SESSION = 1,
        MORONET8_PACKET_SESSION_CREATED,
        MORONET8_PACKET_JOIN_SESSION,
        MORONET8_PACKET_SESSION_JOINED,
        MORONET8_PACKET_LEAVE_SESSION,
        MORONET8_PACKET_SESSION_LEFT,
        MORONET8_PACKET_BROADCAST,
        MORONET8_PACKET_SEND_TO,
        MORONET8_PACKET_DATA,
        MORONET8_PACKET_KEEP_ALIVE
    };

    struct moronet8_packet_reader
    {
        const moronet8_u8 *buf;
        size_t size;
        size_t offset;
    };

    struct moronet8_packet_writer
    {
        moronet8_u8 *buf;
        size_t size;
        size_t offset;
    };

    MORONET8_PUBLIC(struct moronet8_packet_reader *)
    moronet8_packet_reader_create(const moronet8_u8 *buf, size_t size);

    MORONET8_PUBLIC(struct moronet8_packet_writer *)
    moronet8_packet_writer_create(moronet8_u8 *buf, size_t size);

    MORONET8_PUBLIC(void)
    moronet8_packet_reader_init(struct moronet8_packet_reader *reader, const moronet8_u8 *buf, size_t size);

    MORONET8_PUBLIC(void)
    moronet8_packet_writer_init(struct moronet8_packet_writer *writer, moronet8_u8 *buf, size_t size);

    MORONET8_PUBLIC(moronet8_u8)
    moronet8_packet_read_u8(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_u8(struct moronet8_packet_writer *writer, moronet8_u8 value);

    MORONET8_PUBLIC(moronet8_u16)
    moronet8_packet_read_u16(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_u16(struct moronet8_packet_writer *writer, moronet8_u16 value);

    MORONET8_PUBLIC(moronet8_u32)
    moronet8_packet_read_u32(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_u32(struct moronet8_packet_writer *writer, moronet8_u32 value);

    MORONET8_PUBLIC(moronet8_s8)
    moronet8_packet_read_s8(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_s8(struct moronet8_packet_writer *writer, moronet8_s8 value);

    MORONET8_PUBLIC(moronet8_s16)
    moronet8_packet_read_s16(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_s16(struct moronet8_packet_writer *writer, moronet8_s16 value);

    MORONET8_PUBLIC(moronet8_s32)
    moronet8_packet_read_s32(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_write_s32(struct moronet8_packet_writer *writer, moronet8_s32 value);

    MORONET8_PUBLIC(size_t)
    moronet8_packet_read_string(struct moronet8_packet_reader *reader, char *buf, size_t size);

    MORONET8_PUBLIC(size_t)
    moronet8_packet_write_string(struct moronet8_packet_writer *writer, const char *buf, size_t size);

    MORONET8_PUBLIC(void)
    moronet8_packet_reader_seek(struct moronet8_packet_reader *reader, size_t offset);

    MORONET8_PUBLIC(void)
    moronet8_packet_writer_seek(struct moronet8_packet_writer *writer, size_t offset);

    MORONET8_PUBLIC(size_t)
    moronet8_packet_reader_size(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(size_t)
    moronet8_packet_writer_size(struct moronet8_packet_writer *writer);

    MORONET8_PUBLIC(const moronet8_u8 *)
    moronet8_packet_reader_buffer(struct moronet8_packet_reader *reader, size_t *size);

    MORONET8_PUBLIC(moronet8_u8 *)
    moronet8_packet_writer_buffer(struct moronet8_packet_writer *writer, size_t *size);

    MORONET8_PUBLIC(void)
    moronet8_packet_reader_delete(struct moronet8_packet_reader *reader);

    MORONET8_PUBLIC(void)
    moronet8_packet_writer_delete(struct moronet8_packet_writer *writer);

#ifdef __cplusplus
}
#endif
