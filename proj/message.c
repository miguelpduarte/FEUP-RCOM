#include "message.h"
#include "message_defines.h"
#include "utils.h"
#include "state.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "dyn_buffer.h"

static size_t num_rejs;
static size_t num_rrs;

int writeSupervisionMessage(int fd, byte msg_addr, byte msg_ctrl) {
    byte msg_buf[MSG_SUPERVISION_MSG_SIZE];

    msg_buf[MSG_FLAG_START_IDX] = MSG_FLAG;
    msg_buf[MSG_ADDR_IDX] = msg_addr;
    msg_buf[MSG_CTRL_IDX] = msg_ctrl;
    msg_buf[MSG_BCC1_IDX] = MSG_BCC1(msg_addr, msg_ctrl);
    msg_buf[MSG_FLAG_END_IDX] = MSG_FLAG;

    return write(fd, msg_buf, MSG_SUPERVISION_MSG_SIZE);
}

int writeInfoMessage(int fd, const info_message_details_t info_message_details, byte * data, size_t data_size) {
    if(data_size == 0) {
        return -1;
    }

    byte * msg_buf = malloc(MSG_INFO_MSG_SIZE(data_size) * sizeof(*msg_buf));

    msg_buf[MSG_FLAG_START_IDX] = MSG_FLAG;
    msg_buf[MSG_ADDR_IDX] = info_message_details.addr;
    msg_buf[MSG_CTRL_IDX] = MSG_CTRL_S(info_message_details.msg_nr);
    msg_buf[MSG_BCC1_IDX] = MSG_BCC1(info_message_details.addr, MSG_CTRL_S(info_message_details.msg_nr));

    memcpy(msg_buf + MSG_DATA_BASE_IDX, data, data_size);

    u_short stuffedBcc2 = stuffByte(info_message_details.bcc2);

    // If BCC2 needs stuffing, flag has to be shifted
    if (GET_SHORT_MSB(stuffedBcc2) == MSG_ESCAPE_BYTE) {
        msg_buf[MSG_BCC2_IDX(data_size)] = GET_SHORT_MSB(stuffedBcc2);
        msg_buf[MSG_BCC2_IDX(data_size)+1] = GET_SHORT_LSB(stuffedBcc2);
        msg_buf[MSG_INFO_FLAG_END_IDX(data_size)+1] = MSG_FLAG;
    } else {
        msg_buf[MSG_BCC2_IDX(data_size)] = GET_SHORT_LSB(stuffedBcc2);
        msg_buf[MSG_INFO_FLAG_END_IDX(data_size)] = MSG_FLAG;
    }

    int ret = write(fd, msg_buf, MSG_INFO_MSG_SIZE(data_size));
    free(msg_buf);
    
    //TODO: Document
    //Returns 0 if success - the full buffer was written
    return ret != MSG_INFO_MSG_SIZE(data_size);
}

byte readInfoMsgResponse(int fd, byte msg_nr_S) {
    //if rej or cant read return msg_nr_S again
    //else, porque rr retorna o que vier no rr
    // - no need to worry with checking if its the same, that's in the above layer
    int ret = readSupervisionMessage(fd);

    if (ret != MSG_SUPERVISION_MSG_SIZE) {
        // Response got lost
        return msg_nr_S;
    } else {
        byte msg_ctrl = getMsgCtrl();

        if (msg_ctrl == MSG_CTRL_RR_0 || msg_ctrl == MSG_CTRL_RR_1) {
            num_rrs++;
            return MSG_CTRL_RR_DECODE(msg_ctrl);
        } else if (msg_ctrl == MSG_CTRL_REJ_0 || msg_ctrl == MSG_CTRL_REJ_1) {
            num_rejs++;
            return RECEIVED_REJ;
        } else {
            return msg_nr_S;
        }
    }
}

int readSupervisionMessage(int fd) {
    resetMsgState();
    byte msg_byte;
    ssize_t ret;
    while(getState() != SUP_MSG_RECEIVED) {
        // Ensure the message received is a supervision message
        if (getState() == INFO_MSG_RECEIVED || getState() == MSG_ERROR) {
            resetMsgState();
        }

        ret = read(fd, &msg_byte, 1);

        if (ret <= 0) {
            return ret;
        }

        handleMsgByte(msg_byte); 
    }

    return MSG_SUPERVISION_MSG_SIZE;
}

int receiverRead(int fd, dyn_buffer_st * dyn_buffer) {
    resetMsgState();
    byte msg_byte;
    ssize_t ret;
    int time_out_counter = 0;
    size_t read_buf_size, msg_nr = 0;
    byte * read_buf;

    while(1) {
        ret = read(fd, &msg_byte, 1);

        if (ret <= 0) {
            if(time_out_counter < MSG_NUM_READ_TRIES) {
                //Timed out in reading, resetting mesage state for safety 
                // and sending an RR for the same message
                resetMsgState();
                time_out_counter++;
                continue;
            } else {
                return RECEIVER_READ_TIMEOUT;
            }
        } else {
            time_out_counter = 0;
        }

        handleMsgByte(msg_byte);

        if (getState() == SUP_MSG_RECEIVED) {
            if (getMsgCtrl() == MSG_CTRL_DISC) {
                return RECEIVER_READ_DISC;
            }
        } else if (getState() == INFO_MSG_RECEIVED) {
            if(getMsgCtrl() == MSG_CTRL_S(msg_nr)) {
                //Received the message we were expecting
                read_buf = getInfoMsgBuffer(&read_buf_size);

                concatBuffer(dyn_buffer, read_buf, read_buf_size);

                // ready to receive next message
                msg_nr++;
            }
    
            resetMsgState();
            //Writing the correct RR ()
            writeSupWithRetry(fd, MSG_ADDR_EMT, MSG_CTRL_RR(msg_nr));
            num_rrs++;

        } else if (getState() == MSG_ERROR) {
            resetMsgState();

            // ask for message resend
            writeSupWithRetry(fd, MSG_ADDR_EMT, MSG_CTRL_REJ(msg_nr));
            num_rejs++;
        }
    }

    return RECEIVER_READ_DISC;
}

int writeSupWithRetry(int fd, byte addr, byte ctrl) {
    int num_tries, ret;
    for (num_tries = 0; num_tries < MSG_NUM_RESEND_TRIES; num_tries++) {
        ret = writeSupervisionMessage(fd, addr, ctrl);

        // If the whole message could not be written, retry to write it
        if (ret != MSG_SUPERVISION_MSG_SIZE && num_tries < MSG_NUM_RESEND_TRIES - 1) {
            sleep(MSG_RESEND_DELAY);
            continue;
        } else {
            return 0;
        }
    }

    return WRITE_SUP_MSG_FAILED;
}

size_t getNumRRs(){
    return num_rrs;
}

size_t getNumRejs(){
    return num_rejs;
}