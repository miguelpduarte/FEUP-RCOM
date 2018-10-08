#ifndef _SUPERVISION_STATE_
#define _SUPERVISION_STATE_

#include "message_defines.h"

typedef enum {WAITING_FLAG=0, FLAG_RCV, ADDR_RCV, CTRL_RCV, BCC_OK, MSG_RECEIVED} state_st;

typedef struct {
    state_st current_state;
    byte addr;
    byte ctrl;
} state_machine_st;

state_st getState();

void handleMsgByte(byte msg_byte);

#endif /* _SUPERVISION_STATE_ */