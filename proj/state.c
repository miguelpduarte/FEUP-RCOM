#include "state.h"

static void setState(state_st new_state);

static void handleWaitingFlag(byte msg_byte);
static void handleFlagReceived(byte msg_byte);
static void handleAddrReceived(byte msg_byte);

static void handleSupCtrlReceived(byte msg_byte);
static void handleBccOk(byte msg_byte);

static state_machine_st state_machine = (state_machine_st){.msg_index = 0, .current_state = WAITING_FLAG};

byte * getInfoMsgBuffer(size_t * msg_size) {
    //Because we are ignoring the bcc2 which will end up in the buffer
    *msg_size = state_machine.msg_index - 1;
}

state_st getInfoMsgState() {
    return state_machine.current_state;
}

byte getMsgCtrl() {
    return state_machine.ctrl;
}

void resetMsgState() {
    state_machine.current_state = WAITING_FLAG;
    state_machine.msg_index = 0;
}

/*
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
*/

static void setState(state_st new_state) {
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
    byte bcc2 = state_machine.msg[state_machine.msg_index - 1];
    //Check bcc2
    byte calculatedBcc2 = calcBcc2(state_machine.msg, state_machine.msg_index - 1);

    if(bcc2 == calculatedBcc2) {
        setState(INFO_MSG_RECEIVED);
    } else {
        setState(MSG_ERROR);
    }
}

//TODO: Refactor or call function from elsewhere - utils file?
static byte calcBcc2(byte * data, const size_t data_size) {
    byte bcc2 = data[0];
    size_t i = 1;

    for(; i < data_size; ++i) {
        bcc2 ^= data[i];
    }

    return bcc2;
}