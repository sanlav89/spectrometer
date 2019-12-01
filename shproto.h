#ifndef SHPROTO_H
#define SHPROTO_H
// higher bit must be set to avoid ONE_CMD == ~ANOTHER_CMD
#define SHPROTO_START   (0xFE | 0x80)
#define SHPROTO_ESC     (0xFD | 0x80)
#define SHPROTO_FINISH  (0xA5 | 0x80)

typedef struct {
    unsigned char * data;
    unsigned short buffer_size;
    unsigned short len;
    unsigned short crc;
    unsigned char ready;
    unsigned char complete;
    unsigned char cmd;
    unsigned char esc;
} shproto_struct;

#define shproto_buffer_release(buffer)  buffer.ready = 0

void shproto_packet_start(
        shproto_struct * shproto_packet,
        unsigned char cmd
        );
void shproto_packet_add_data(
        shproto_struct * shproto_packet,
        unsigned char tx_byte
        );
unsigned short shproto_packet_complete(
        shproto_struct * shproto_packet
        );
void shproto_byte_received(
        shproto_struct * shproto_packet,
        unsigned char rx_byte
        );

#endif
