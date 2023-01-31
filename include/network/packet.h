#pragma once

#include "morobox8_config.h"
#include "morobox8_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum morobox8_packet_type
    {
        MOROBOX8_PACKET_CREATE_SESSION = 1,
        MOROBOX8_PACKET_SESSION_CREATED,
        MOROBOX8_PACKET_JOIN_SESSION,
        MOROBOX8_PACKET_SESSION_JOINED,
        MOROBOX8_PACKET_LEAVE_SESSION,
        MOROBOX8_PACKET_SESSION_LEFT,
        MOROBOX8_PACKET_BROADCAST,
        MOROBOX8_PACKET_SEND_TO,
        MOROBOX8_PACKET_DATA,
        MOROBOX8_PACKET_KEEP_ALIVE
    };

    struct morobox8_packet_reader
    {
        const morobox8_u8 *buf;
        size_t size;
        size_t offset;
    };

    struct morobox8_packet_writer
    {
        morobox8_u8 *buf;
        size_t size;
        size_t offset;
    };

    MOROBOX8_PUBLIC(struct morobox8_packet_reader *)
    morobox8_packet_reader_create(const morobox8_u8 *buf, size_t size);

    MOROBOX8_PUBLIC(struct morobox8_packet_writer *)
    morobox8_packet_writer_create(morobox8_u8 *buf, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_reader_init(struct morobox8_packet_reader *reader, const morobox8_u8 *buf, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_writer_init(struct morobox8_packet_writer *writer, morobox8_u8 *buf, size_t size);

    MOROBOX8_PUBLIC(morobox8_u8)
    morobox8_packet_read_u8(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_u8(struct morobox8_packet_writer *writer, morobox8_u8 value);

    MOROBOX8_PUBLIC(morobox8_u16)
    morobox8_packet_read_u16(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_u16(struct morobox8_packet_writer *writer, morobox8_u16 value);

    MOROBOX8_PUBLIC(morobox8_u32)
    morobox8_packet_read_u32(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_u32(struct morobox8_packet_writer *writer, morobox8_u32 value);

    MOROBOX8_PUBLIC(morobox8_s8)
    morobox8_packet_read_s8(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_s8(struct morobox8_packet_writer *writer, morobox8_s8 value);

    MOROBOX8_PUBLIC(morobox8_s16)
    morobox8_packet_read_s16(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_s16(struct morobox8_packet_writer *writer, morobox8_s16 value);

    MOROBOX8_PUBLIC(morobox8_s32)
    morobox8_packet_read_s32(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_write_s32(struct morobox8_packet_writer *writer, morobox8_s32 value);

    MOROBOX8_PUBLIC(size_t)
    morobox8_packet_read_string(struct morobox8_packet_reader *reader, char *buf, size_t size);

    MOROBOX8_PUBLIC(size_t)
    morobox8_packet_write_string(struct morobox8_packet_writer *writer, const char *buf, size_t size);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_reader_seek(struct morobox8_packet_reader *reader, size_t offset);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_writer_seek(struct morobox8_packet_writer *writer, size_t offset);

    MOROBOX8_PUBLIC(size_t)
    morobox8_packet_reader_size(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(size_t)
    morobox8_packet_writer_size(struct morobox8_packet_writer *writer);

    MOROBOX8_PUBLIC(const morobox8_u8 *)
    morobox8_packet_reader_buffer(struct morobox8_packet_reader *reader, size_t *size);

    MOROBOX8_PUBLIC(morobox8_u8 *)
    morobox8_packet_writer_buffer(struct morobox8_packet_writer *writer, size_t *size);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_reader_delete(struct morobox8_packet_reader *reader);

    MOROBOX8_PUBLIC(void)
    morobox8_packet_writer_delete(struct morobox8_packet_writer *writer);

#ifdef __cplusplus
}
#endif
