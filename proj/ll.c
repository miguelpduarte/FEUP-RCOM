#include "ll.h"
#include "message.h"
#include "message_defines.h"
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

static int writeAndRetry(const int fd, const info_message_details_t info_message_details, byte * stuffed_data, const size_t stuffed_data_size);

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
            } else {
                return 1;
            }
        }

        return ESTABLISH_DATA_CONNECTION_FAILED;        
    } else if (role == RECEIVER) {
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
    } else {
        return INVALID_COMMUNICATION_ROLE;
    }

    return 1;
}

int llwrite(int fd, byte* buffer, const size_t length) {
    size_t num_bytes_written = 0;
    data_stuffing_t ds;

    info_message_details_t msg_details;
    msg_details.msg_nr = 0;
    msg_details.addr = MSG_ADDR_EMT;

    byte stuffed_data_buffer[MSG_STUFFING_BUFFER_SIZE];

    do {
        ds = stuffData(buffer, length, num_bytes_written, stuffed_data_buffer);
        msg_details.bcc2 = calcBcc2(buffer, ds.data_bytes_stuffed, num_bytes_written);
        
        //Updating current "index" in data buffer
        num_bytes_written += ds.data_bytes_stuffed;

        //Write message and proceed accordingly to return
        if(writeAndRetry(fd, msg_details, stuffed_data_buffer, ds.stuffed_buffer_size) != 0) {
            return LLWRITE_FAILED;
        }

        msg_details.msg_nr++;

    } while(num_bytes_written < length);

    return 0;
}

static int writeAndRetry(const int fd, const info_message_details_t info_message_details, byte * stuffed_data, const size_t stuffed_data_size) {
    int current_attempt = 0;
    int response = 0, num_bytes_written;
    const byte msg_nr_S = MSG_CTRL_S(info_message_details.msg_nr);

    do {
        current_attempt++;

        num_bytes_written = writeInfoMessage(fd, info_message_details, stuffed_data, stuffed_data_size);
        if(num_bytes_written != stuffed_data_size) {
            continue;
        }

        response = readInfoMsgResponse(fd, msg_nr_S);
        if(response == msg_nr_S) {
            continue;
        } else {
            return 0;
        }

    } while(current_attempt < MSG_NUM_RESEND_TRIES);

    return 1;    
}
