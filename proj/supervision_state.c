#include "supervision_state.h"

static void handleWaitingFlag(byte msg_byte);
static void handleFlagReceived(byte msg_byte);
static void handleAddrReceived(byte msg_byte);
static void handleCtrlReceived(byte msg_byte);
static void handleBccOk(byte msg_byte);
static void setState(supervision_state_st new_state);

static supervision_state_machine_st state_machine = (supervision_state_machine_st){.current_state = WAITING_FLAG};

supervision_state_st getState() {
    return state_machine.current_state;
}

// WAITING_FLAG, FLAG_RCV, ADDR_RCV, CTRL_RCV, BCC_OK, MSG_RECEIVED

void handleSupMsgByte(byte msg_byte) {
    switch (state_machine.current_state) {
        case WAITING_FLAG:
            handleWaitingFlag(msg_byte);
            break;
        case FLAG_RCV:
            handleFlagReceived(msg_byte);
            break;
        case ADDR_RCV:
            handleAddrReceived(msg_byte);
            break;
        case CTRL_RCV:
            handleCtrlReceived(msg_byte);
            break;
        case BCC_OK:
            handleBccOk(msg_byte);
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
            state_machine.addr = msg_byte;
            break;
        default:
            setState(WAITING_FLAG);
            break;
    }
}

static void handleAddrReceived(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            setState(FLAG_RCV);
            break;
        case MSG_CTRL_DISC: case MSG_CTRL_SET: 
        case MSG_CTRL_UA:
        //TODO remaining controls with parameter
            setState(CTRL_RCV);
            state_machine.ctrl = msg_byte;
            break;
        default:
            setState(WAITING_FLAG);
            break;    
    }
}

static void handleCtrlReceived(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            setState(FLAG_RCV);
            break;
        default:
            if(msg_byte == MSG_BCC1(state_machine.addr, state_machine.ctrl)) {
                setState(BCC_OK);
            } else {
                setState(WAITING_FLAG);
            }
            break;
    }
}

static void handleBccOk(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            setState(MSG_RECEIVED);
            break;
        default:
            setState(WAITING_FLAG);
            break;
    }
}

static void setState(supervision_state_st new_state) {
    state_machine.current_state = new_state;
}