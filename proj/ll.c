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
                // TODO: Find out what to do with connection id
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

    // TODO: Find out what to do with connection id
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
            //Oh shit, the whole message was not written
            //current_attempt++;
            //continue; Should we just re-write? - confirm with teacher
        }

        response = readInfoMsgResponse(fd, msg_nr_S);
        //this if is ugly, for clarity
        if(response == msg_nr_S) {
            //resend msg
            continue;
        } else {
            return 0;
        }

    } while(current_attempt < MSG_NUM_RESEND_TRIES);

    return 1;    
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

data_unstuffing_t unstuffData(byte * data, const size_t data_size, const size_t data_start_index, byte * unstuffed_buffer) {
    size_t data_index = data_start_index, unstuffed_buffer_index = 0;

    while(unstuffed_buffer_index < MSG_PART_MAX_SIZE && data_index < data_size) {
        if (data[data_index] == MSG_ESCAPE_BYTE) {
            data_index++;
            if (data[data_index] == MSG_FLAG_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_FLAG;
            } else if (data[data_index] == MSG_ESCAPE_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            } else {
                // TODO: What to do in this case? This is not supposed to happen if everything goes well according to the protocol :/
                exit(-1);
            }
        }
        else {
            unstuffed_buffer[unstuffed_buffer_index++] = data[data_index];
        }

        data_index++;   // TODO: this could be a for loop, but the thought process is much easier and more coherent with a while loop IMO
    }

    data_unstuffing_t dus;
    dus.data_bytes_unstuffed = data_index - data_start_index;
    dus.unstuffed_buffer_size = unstuffed_buffer_index;

    return dus;
}
