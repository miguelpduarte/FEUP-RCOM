#ifndef _INFO_STATE_
#define _INFO_STATE_

#include "message_defines.h"

typedef enum {
    WAITING_FLAG=0,
    FLAG_RCV,
    ADDR_RCV,

    SUP_CTRL_RCV,
    SUP_BCC_OK,
    SUP_MSG_RECEIVED,

    INFO_CTRL_RCV,
    RECEIVING_DATA,
    
    MSG_ERROR,          //Message error, send respective REJ
    INFO_MSG_RECEIVED   //Message received successfully
} state_st;

typedef struct {
    state_st current_state;
    byte addr;
    byte ctrl;
    size_t msg_index;
    byte msg[MSG_INFO_RECEIVER_BUFFER_SIZE];
} state_machine_st;

state_st getState();

byte getMsgCtrl();

void handleMsgByte(byte msg_byte);

byte * getInfoMsgBuffer(size_t * msg_size);

void resetMsgState();

#endif /* _INFO_STATE_ */