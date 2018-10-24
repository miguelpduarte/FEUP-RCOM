#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "message_defines.h"

byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index) {
    byte bcc2 = data[data_start_index];
    size_t i = data_start_index + 1;

    for(; i < data_start_index + data_size; ++i) {
        bcc2 ^= data[i];
    }

    return bcc2;
}

data_stuffing_t stuffData(byte * data, const size_t data_size, const size_t data_start_index, byte * stuffed_buffer) {
    size_t data_index = data_start_index, stuffed_buffer_index = 0;
    size_t num_loops = MIN(MSG_PART_MAX_SIZE, (ssize_t) (data_size - data_start_index));

    for (; data_index < data_start_index + num_loops; data_index++) {
        if (data[data_index] == MSG_FLAG) {
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            stuffed_buffer[stuffed_buffer_index++] = MSG_FLAG_STUFFING_BYTE;
        } else if (data[data_index] == MSG_ESCAPE_BYTE) {
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_STUFFING_BYTE;
        } else {
            stuffed_buffer[stuffed_buffer_index++] = data[data_index];
        }
    }

    data_stuffing_t ds;
    ds.data_bytes_stuffed = num_loops;
    ds.stuffed_buffer_size = stuffed_buffer_index;

    return ds;
}

ssize_t unstuffData(byte * data, const size_t data_size, byte * unstuffed_buffer) {
    size_t data_index = 0, unstuffed_buffer_index = 0;

    while(data_index < data_size) {
        if (data[data_index] == MSG_ESCAPE_BYTE) {
            data_index++;

            if (data[data_index] == MSG_FLAG_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_FLAG;
            } else if (data[data_index] == MSG_ESCAPE_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            } else {
                printf("Found invalid byte after escape byte! Unsure on how to proceed!\n");
                // Found invalid byte after escape byte,    
                return -1;
            }
        } else {
            unstuffed_buffer[unstuffed_buffer_index++] = data[data_index];
        }

        data_index++;
    }

    return unstuffed_buffer_index;
}

u_short stuffByte(byte b) {
    u_short stuffedByte;

    if (b == MSG_FLAG) {
        stuffedByte = (MSG_ESCAPE_BYTE << 8) | MSG_FLAG_STUFFING_BYTE;
    } else if (b == MSG_ESCAPE_BYTE) {
        stuffedByte = (MSG_ESCAPE_BYTE << 8) | MSG_ESCAPE_STUFFING_BYTE;
    } else {
        stuffedByte = b;
    }

    return stuffedByte;
}