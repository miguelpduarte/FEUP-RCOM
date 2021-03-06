#include <unistd.h>
#include "state.h"
#include "utils.h"

static void setState(state_t new_state);

static void handleWaitingFlag(byte msg_byte);
static void handleFlagReceived(byte msg_byte);
static void handleAddrReceived(byte msg_byte);

static void handleSupCtrlReceived(byte msg_byte);
static void handleBccOk(byte msg_byte);
static void handleReceivingData(byte msg_byte);
static void handleInfoCtrlReceived(byte msg_byte);
static void handleFinalFlagRcv();

static state_machine_st state_machine = (state_machine_st){.msg_index = 0, .current_state = WAITING_FLAG, .unstuffed_msg_size = 0};

byte * getInfoMsgBuffer(size_t * msg_size) {
    //Ignoring the bcc2 which will end up in the buffer
    *msg_size = state_machine.unstuffed_msg_size - 1;
    return state_machine.unstuffed_msg;
}

state_t getState() {
    return state_machine.current_state;
}

byte getMsgCtrl() {
    return state_machine.ctrl;
}

void resetMsgState() {
    state_machine.current_state = WAITING_FLAG;
    state_machine.msg_index = 0;
}

static void setState(state_t new_state) {
    state_machine.current_state = new_state;
}

void handleMsgByte(byte msg_byte) {
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

        case SUP_CTRL_RCV:
            handleSupCtrlReceived(msg_byte);
            break;
        case SUP_BCC_OK:
            handleBccOk(msg_byte);
            break;
        case SUP_MSG_RECEIVED:
            return;

        case INFO_CTRL_RCV:
            handleInfoCtrlReceived(msg_byte);
            break;
        case RECEIVING_DATA:
            handleReceivingData(msg_byte);
            break;

        case MSG_ERROR:
            return;
        case INFO_MSG_RECEIVED:
            return;
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
        case MSG_CTRL_DISC:
        case MSG_CTRL_SET: 
        case MSG_CTRL_UA:
        case MSG_CTRL_RR_0:
        case MSG_CTRL_RR_1:
        case MSG_CTRL_REJ_0:
        case MSG_CTRL_REJ_1:
            setState(SUP_CTRL_RCV);
            state_machine.ctrl = msg_byte;
            break;
        case MSG_CTRL_S0: case MSG_CTRL_S1:
            setState(INFO_CTRL_RCV);
            state_machine.ctrl = msg_byte;
            break;
        default:
            setState(WAITING_FLAG);
            break;    
    }
}

static void handleSupCtrlReceived(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            setState(FLAG_RCV);
            break;
        default:
            if(msg_byte == MSG_BCC1(state_machine.addr, state_machine.ctrl)) {
                setState(SUP_BCC_OK);
            } else {
                setState(WAITING_FLAG);
            }
            break;    
    }
}

static void handleBccOk(byte msg_byte) {
    switch (msg_byte) {
        case MSG_FLAG:
            setState(SUP_MSG_RECEIVED);
            break;
        default:
            setState(WAITING_FLAG);
            break;
    }
}

static void handleInfoCtrlReceived(byte msg_byte) {
    switch(msg_byte) {
        case MSG_FLAG:
            setState(FLAG_RCV);
            break;
        default:
            if(msg_byte == MSG_BCC1(state_machine.addr, state_machine.ctrl)) {
                setState(RECEIVING_DATA);
            } else {
                setState(WAITING_FLAG);
            }
            break;
    }
}

static void handleReceivingData(byte msg_byte) {
    switch(msg_byte) {
        case MSG_FLAG:
            handleFinalFlagRcv();
            break;
        default:
            if(state_machine.msg_index < MSG_INFO_RECEIVER_BUFFER_SIZE) {
                state_machine.msg[state_machine.msg_index++] = msg_byte;
            } else {
                setState(MSG_ERROR);
            }
            break;
    }
}

static void handleFinalFlagRcv() {
    // First, unstuff the message
    ssize_t res = unstuffData(state_machine.msg, state_machine.msg_index, state_machine.unstuffed_msg);

    if (res < 0) {
        setState(MSG_ERROR);
        return;
    }

    state_machine.unstuffed_msg_size = (size_t) res;

    // Then, calculate the bcc2 of the unstuffed message
    // (Subtracting 1 to not count bcc2 in the bcc2 calculation)
    byte calculated_bcc2 = calcBcc2(state_machine.unstuffed_msg, state_machine.unstuffed_msg_size - 1, 0);

    // Finally, compare the calculated bcc2 to the final byte of the unstuffed message (received bcc2)
    byte received_bcc2 = state_machine.unstuffed_msg[state_machine.unstuffed_msg_size - 1];

    if (calculated_bcc2 == received_bcc2) {
        setState(INFO_MSG_RECEIVED);
    } else {
        setState(MSG_ERROR);
    }
}