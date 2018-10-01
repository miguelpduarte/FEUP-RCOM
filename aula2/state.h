#ifndef _STATE_
#define _STATE_

#include "message_defines.h"

typedef enum {WAITING_FLAG, FLAG_RCV, ADDR_RCV, CTRL_RCV, BCC_OK, MSG_RECEIVED} state_st;

typedef struct {
    state_st current_state;
    byte addr;
    byte ctrl;
} state_machine_st;

state_st getState();

void handleEvent(byte msg_byte);

#endif /* _STATE_ */