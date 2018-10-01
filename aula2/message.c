#include "message.h"
#include "message_defines.h"

void buildSupervisionMessage(byte msg_addr, byte msg_ctrl, byte * msg) {
    msg[MSG_FLAG_START_IDX] = MSG_FLAG;
    msg[MSG_ADDR_IDX] = msg_addr;
    msg[MSG_CTRL_IDX] = msg_ctrl;
    msg[MSG_BCC1_IDX] = MSG_BCC1(msg_addr, msg_ctrl);
    msg[MSG_FLAG_END_IDX] = MSG_FLAG;
}