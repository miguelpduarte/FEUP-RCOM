#include "state.h"

static state_machine_st state_machine = (state_machine_st){.current_state = WAITING_FLAG};

state_st getState() {
    return state_machine.current_state;
}

// WAITING_FLAG, FLAG_RCV, ADDR_RCV, CTRL_RCV, BCC_OK, MSG_RECEIVED

void handleEvent(byte msg_byte) {
    switch (state_machine.current_state) {
        case WAITING_FLAG:
            handleWaitingFlag(msg_byte);
            break;
        case FLAG_RCV:
            handleFlagReceived(msg_byte);
            break;
        case ADDR_RCV:
            break;
        case CTRL_RCV:
            break;
        case BCC_OK:
            break;
        case MSG_RECEIVED:
            return;
        default:
            break;
    }
}

static void handleWaitingFlag(byte msg_byte) {
    switch(msg_byte) {
        case MSG_FLAG:
            setState(FLAG_RCV);
            break;
        default:
            break;
    }
}

static void handleFlagReceived(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            break;
        case MSG_ADDR_EMT: case MSG_ADDR_REC:
            setState(ADDR_RCV);
            break;
        default:
            setState(WAITING_FLAG);
            break;
    }
}

static void setState(state_st new_state) {
    state_machine.current_state = new_state;
}