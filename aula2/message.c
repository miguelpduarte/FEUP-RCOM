#include "message.h"
#include "message_defines.h"
#include <unistd.h>

/* void buildSupervisionMessage(byte msg_addr, byte msg_ctrl, byte * msg) {
    msg[MSG_FLAG_START_IDX] = MSG_FLAG;
    msg[MSG_ADDR_IDX] = msg_addr;
    msg[MSG_CTRL_IDX] = msg_ctrl;
    msg[MSG_BCC1_IDX] = MSG_BCC1(msg_addr, msg_ctrl);
    msg[MSG_FLAG_END_IDX] = MSG_FLAG;
} */

int writeSupervisionMessage(int fd, byte msg_addr, byte msg_ctrl) {
    byte msg_buf[MSG_SUPERVISION_MSG_SIZE];

    msg_buf[MSG_FLAG_START_IDX] = MSG_FLAG;
    msg_buf[MSG_ADDR_IDX] = msg_addr;
    msg_buf[MSG_CTRL_IDX] = msg_ctrl;
    msg_buf[MSG_BCC1_IDX] = MSG_BCC1(msg_addr, msg_ctrl);
    msg_buf[MSG_FLAG_END_IDX] = MSG_FLAG;

    return write(fd, msg_buf, MSG_SUPERVISION_MSG_SIZE);
}

int readSupervisionMessage(int fd, byte* buffer) {
    byte msg_byte;
    int ret;
    while(getState() != MSG_RECEIVED) {
        ret = readMessageByte(fd, &msg_byte);

        if (ret <= 0) {
            return ret;
        }

        // read the correct byte according to the current state
        buffer[getState()] = msg_byte;

        handleMsgByte(msg_byte); 
    }

    return MSG_SUPERVISION_MSG_SIZE;
}

int readMessageByte(int fd, byte* msg_byte) {
    int num_tries;
    ssize_t num_read_bytes;
    for(num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
        num_read_bytes = read(fd, msg_byte, 1);

        if (num_read_bytes > 0) {
            break;
        }
    }

    return num_read_bytes;
}

