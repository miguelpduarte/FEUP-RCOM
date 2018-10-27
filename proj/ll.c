#include "ll.h"
#include "message.h"
#include "message_defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "state.h"


static info_message_details_t msg_details = (info_message_details_t){.msg_nr = 0}; 
static int writeAndRetryInfoMsg(const int fd, byte * stuffed_data, const size_t stuffed_data_size);

int llopen(int fd, byte role) {
    if (role == EMITTER) {
        int num_tries, ret;
        for (num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
            ret = writeSupWithRetry(fd, MSG_ADDR_EMT, MSG_CTRL_SET);
            if(ret != 0) {
                continue;
            }

            ret = readSupervisionMessage(fd);
            if(ret != MSG_SUPERVISION_MSG_SIZE || getMsgCtrl() != MSG_CTRL_UA) {
                continue;
            } else {
                return COMMUNICATION_IDENTIFIER;
            }
        }

        return ESTABLISH_DATA_CONNECTION_FAILED;        
    } else if (role == RECEIVER) {
        // Wait until transmitter tries to start communication
        int ret;
        do {
            ret = readSupervisionMessage(fd);

            if (ret != MSG_SUPERVISION_MSG_SIZE) {
                continue;
            }
        } while (getMsgCtrl() != MSG_CTRL_SET);

        ret = writeSupWithRetry(fd, MSG_ADDR_REC, MSG_CTRL_UA);
        if (ret != 0) {
            return WRITE_SUPERVISION_MSG_FAILED;
        }        
    } else {
        return INVALID_COMMUNICATION_ROLE;
    }

    return COMMUNICATION_IDENTIFIER;
}

int llwrite(int fd, byte* buffer, const size_t length) {
    size_t num_bytes_written = 0;
    data_stuffing_t ds;

    msg_details.addr = MSG_ADDR_EMT;

    byte stuffed_data_buffer[MSG_STUFFING_BUFFER_SIZE];

    do {
        ds = stuffData(buffer, length, num_bytes_written, stuffed_data_buffer);
        msg_details.bcc2 = calcBcc2(buffer, ds.data_bytes_stuffed, num_bytes_written);
        
        //Updating current "index" in data buffer
        num_bytes_written += ds.data_bytes_stuffed;

        //Write message and proceed accordingly to return
        if(writeAndRetryInfoMsg(fd, stuffed_data_buffer, ds.stuffed_buffer_size) != 0) {
            return LLWRITE_FAILED;
        }

        msg_details.msg_nr++;

    } while(num_bytes_written < length);

    return 0;
}

static int writeAndRetryInfoMsg(const int fd, byte * stuffed_data, const size_t stuffed_data_size) {
    int current_attempt = 0;
    int response = 0, num_bytes_written;
    const byte msg_nr_S = MSG_CTRL_S(msg_details.msg_nr);

    do {
        current_attempt++;

        num_bytes_written = writeInfoMessage(fd, msg_details, stuffed_data, stuffed_data_size);
        if(num_bytes_written != 0) {
            continue;
        }

        response = readInfoMsgResponse(fd, msg_nr_S);
        if(response == msg_nr_S) {
            continue;
        } else if(response == RECEIVED_REJ) {
            current_attempt = 0;
            continue;
        } else {
            return 0;
        }

    } while(current_attempt < MSG_NUM_RESEND_TRIES);

    return WRITE_AND_RETRY_FAILURE;    
}

int llread(int fd, dyn_buffer_st * dyn_buffer) {
    int num_tries, ret;

    //Exits when it receives a DISC
    ret = receiverRead(fd, dyn_buffer);
    if(ret != RECEIVER_READ_DISC) {
        fprintf(stderr, "Receiver: Timed out in reading!\n");
        return RECEIVER_READ_TIMEOUT;
    }

    for (num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
        // Reply with DISC, confirming connection ending
        ret = writeSupWithRetry(fd, MSG_ADDR_REC, MSG_CTRL_DISC);
        if(ret != 0) {
            continue;
        }

        // Waits for Emitter connection ending, UA
        ret = readSupervisionMessage(fd);
        if(ret != MSG_SUPERVISION_MSG_SIZE || getMsgCtrl() != MSG_CTRL_UA) {
            continue;
        } else {
            return 0;
        }
    }

    return LLREAD_FAILED_CLOSE_COMMUNICATION;
}

int llclose(int fd) {
    //Send DISC
    int num_tries, ret;
    for (num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
        ret = writeSupWithRetry(fd, MSG_ADDR_EMT, MSG_CTRL_DISC);
        if(ret != 0) {
            continue;
        }

        //(Wait for DISC)
        ret = readSupervisionMessage(fd);
        if(ret != MSG_SUPERVISION_MSG_SIZE || getMsgCtrl() != MSG_CTRL_DISC) {
            continue;
        } else {
            break;
        }
    }

    if(num_tries == MSG_NUM_READ_TRIES) {
        fprintf(stderr, "llclose failed, max nr of retries reached!\n");
        return LLCLOSE_DISC_FAILED;
    }

    //Reply with UA
    ret = writeSupWithRetry(fd, MSG_ADDR_EMT, MSG_CTRL_UA);
    if(ret != 0) {
        fprintf(stderr, "llclose failed, could not send UA!\n");
        return LLCLOSE_UA_FAILED;
    }

    return 0;
}