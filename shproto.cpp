#include "shproto.h"

// crc16 (modbus algo)
unsigned short crc16(unsigned short crc, unsigned char data)
{
    unsigned char i;
    crc = crc ^ data;
    for (i = 0; i < 8; ++i) {
        if (crc & 0x0001) {
            crc = (crc >> 1) ^ 0xA001;
        } else {
            crc = (crc >> 1);
        }
    }
    return crc;
}

// add byte to buffer (with escaping)
void shproto_packet_add_data(
        shproto_struct * shproto_packet,
        unsigned char tx_byte
        )
{
    if (shproto_packet->len >= shproto_packet->buffer_size)
        return;
    shproto_packet->crc = crc16(shproto_packet->crc, tx_byte);
    if (tx_byte == SHPROTO_ESC ||
        tx_byte == SHPROTO_START ||
        tx_byte == SHPROTO_FINISH) {
        shproto_packet->data[shproto_packet->len++] = SHPROTO_ESC;
        shproto_packet->data[shproto_packet->len++] = (~tx_byte) & 0xFF;
    } else {
        shproto_packet->data[shproto_packet->len++] = tx_byte;
    }
}

// reset transmit buffer, add initial data
void shproto_packet_start(
        shproto_struct * shproto_packet,
        unsigned char cmd
        )
{
    shproto_packet->len = 0;
    shproto_packet->crc = 0xFFFF; // to get zero in crc16(data+crc)
    shproto_packet->data[shproto_packet->len++] = 0xFF;
    shproto_packet->data[shproto_packet->len++] = SHPROTO_START;
    shproto_packet_add_data(shproto_packet, cmd);
}

// send shproto packet: set data in buffer and start sending
unsigned short shproto_packet_complete(shproto_struct * shproto_packet)
{
    unsigned short crc = shproto_packet->crc;
    shproto_packet_add_data(shproto_packet, (unsigned char)  crc);
    shproto_packet_add_data(shproto_packet, (unsigned char) (crc>>8));
    if (shproto_packet->len >= shproto_packet->buffer_size) return 0;
    shproto_packet->data[shproto_packet->len++] = SHPROTO_FINISH;
    return shproto_packet->len;
}

// receive bytes one by one and places them into struct. set 'ready' flag is we
// got valid packet.
void shproto_byte_received(
        shproto_struct * shproto_packet,
        unsigned char rx_byte
        )
{
    switch (rx_byte) {
    case SHPROTO_START:
        // shproto_packet->ready = 0;
        shproto_packet->complete = 0;
        shproto_packet->len = 0;
        shproto_packet->esc = 0;
        shproto_packet->crc = 0xFFFF;
        break;
    case SHPROTO_ESC:
        shproto_packet->esc = 1;
        break;
    case SHPROTO_FINISH:
        shproto_packet->len -= 3; // minus command (1 byte) and crc16 (2 bytes)
        if (!shproto_packet->crc)
            shproto_packet->ready = 1;  // crc16 with data and crc itself gives
                                        // zero result, if not, CRC error here
        break;
    default:
        if (shproto_packet->esc) {
            shproto_packet->esc = 0;
            rx_byte = ~rx_byte;
        }
        if (shproto_packet->len) {
            shproto_packet->data[shproto_packet->len-1] = rx_byte;
        } else {
            shproto_packet->cmd = rx_byte;
        }
        if (shproto_packet->len < shproto_packet->buffer_size)
            shproto_packet->len++;
        shproto_packet->crc = crc16(shproto_packet->crc, rx_byte);
        break;
    }
    return;
}
