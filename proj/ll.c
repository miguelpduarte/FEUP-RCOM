#include "ll.h"
#include "message.h"
#include "message_defines.h"
#include <stdlib.h>
#include <unistd.h>

static byte calcBcc2(byte * data, const size_t data_size);

int llopen(int fd, byte role) {
    byte buf[MSG_SUPERVISION_MSG_SIZE];

    if (role == EMITTER) {
        int num_tries, ret;
        for (num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
            ret = writeSupervisionMessage(fd, MSG_ADDR_EMT, MSG_CTRL_SET);

            // If the whole message could not be written, retry to write it
            if (ret != MSG_SUPERVISION_MSG_SIZE) {
                continue;
            }

            // Wait for receiver acknowledgement
            ret = readSupervisionMessage(fd, buf);

            // Verify correct receiver message type
            if (ret != MSG_SUPERVISION_MSG_SIZE || buf[MSG_CTRL_IDX] != MSG_CTRL_UA) {
                continue;
            }
            else {
                // TODO: Find out what to do with connection id
                return 1;
            }
        }

        return ESTABLISH_DATA_CONNECTION_FAILED;        
    }
    else if (role == RECEIVER) {
        // Wait until transmitter tries to start communication
        int ret;
        do {
            ret = readSupervisionMessage(fd, buf);

            if (ret != MSG_SUPERVISION_MSG_SIZE) {
                continue;
            }
        } while (buf[MSG_CTRL_IDX] != MSG_CTRL_SET);

        ret = writeSupervisionMessage(fd, MSG_ADDR_REC, MSG_CTRL_UA);
        if (ret != MSG_SUPERVISION_MSG_SIZE) {
            return WRITE_SUPERVISION_MSG_FAILED;
        }        
    }
    else {
        return INVALID_COMMUNICATION_ROLE;
    }

    // TODO: Find out what to do with connection id
    return 1;
}

int llwrite(int fd, byte* buffer, size_t length) {

    //Partes a mensagem

    //Transmites bocado a bocado
    //manda 0:
        //fazer paridade da informação -> Bcc2
        //fazer stuffing
        //enviar para o writeInfoMessage
    //manda 1
        //fazer paridade da informação -> Bcc2
        //fazer stuffing
        //enviar para o writeInfoMessage
    //manda 2

    return -1;
}

static byte calcBcc2(byte * data, const size_t data_size) {
    size_t i = 1;
    byte bcc2 = data[0];

    for(; i < data_size; ++i) {
        bcc2 ^= data[i];
    }

    return bcc2;
}

size_t stuffMessage(byte * data, const size_t data_size, const size_t data_start_index, byte * stuffed_buffer, size_t * num_bits_stuffed) {
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

    *num_bits_stuffed = num_loops;

    return stuffed_buffer_index;
}