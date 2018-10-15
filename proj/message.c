#include "message.h"
#include "message_defines.h"
#include "supervision_state.h"
#include <unistd.h>
#include <stdlib.h>

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
    
    for(size_t i = 0; i < data_size; ++i) {
        msg_buf[MSG_DATA_BASE_IDX + i] = data[i];
    }

    msg_buf[MSG_BCC2_IDX(data_size)] = info_message_details.bcc2;
    msg_buf[MSG_INFO_FLAG_END_IDX(data_size)] = MSG_FLAG;

    int ret = write(fd, msg_buf, MSG_INFO_MSG_SIZE(data_size));
    free(msg_buf);
    return ret;
}

byte readInfoMsgResponse(int fd, byte msg_nr_S) {
    //if rej or cant read return msg_nr_S again
    //else, porque rr retorna o que vier no rr
    // - no need to worry with checking if its the same, that's in the above layer
    byte response[MSG_SUPERVISION_MSG_SIZE];
    int ret = readSupervisionMessage(fd, response);

    if (ret != MSG_SUPERVISION_MSG_SIZE) {
        // Response got lost
        return msg_nr_S;
    } else {
        if (response[MSG_CTRL_IDX] == MSG_CTRL_RR_0 || response[MSG_CTRL_IDX] == MSG_CTRL_RR_1) {
            return MSG_CTRL_RR_DECODE(response[MSG_CTRL_IDX]);
        } else {
            return msg_nr_S;
        }
    }
}

int readSupervisionMessage(int fd, byte* buffer) {
    byte msg_byte;
    ssize_t ret;
    while(getState() != MSG_RECEIVED) {
        ret = read(fd, &msg_byte, 1);

        if (ret <= 0) {
            return ret;
        }

        // read the correct byte according to the current state
        buffer[getState()] = msg_byte;

        handleMsgByte(msg_byte); 
    }

    return MSG_SUPERVISION_MSG_SIZE;
}