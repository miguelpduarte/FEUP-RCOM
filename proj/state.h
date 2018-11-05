#ifndef _INFO_STATE_
#define _INFO_STATE_

#include "message_defines.h"
#include "utils.h"

/** Enumeration regarding state machine states */
typedef enum {
    WAITING_FLAG=0,     /** Waiting Flag byte */
    FLAG_RCV,           /** Flag Received, waiting address */
    ADDR_RCV,           /** Address Received, waiting control octet */

    SUP_CTRL_RCV,       /** Supervision Message type - Control octet received, waiting BCC */
    SUP_BCC_OK,         /** Supervision Message type - BCC received, waiting Flag to complete message */
    SUP_MSG_RECEIVED,   /** Supervision Message type - Message received successfully */

    INFO_CTRL_RCV,      /** Info Message type - Control octet received, waiting BCC1 */
    RECEIVING_DATA,     /** Info Message type - Receiving data bytes until Flag is received */
    
    MSG_ERROR,          /** Info Message type - Message error, send respective REJ */
    INFO_MSG_RECEIVED   /** Info Message type - Message received successfully */
} state_t;

/** State Machine structure */
typedef struct {
    state_t current_state;                     /** Current machine state */
    byte addr;                                  /** Message address */
    byte ctrl;                                  /** Message control octet */
    size_t msg_index;                           /** Info message buffer current index (size in bytes) */
    byte msg[MSG_INFO_RECEIVER_BUFFER_SIZE];    /** Info message buffer (still stuffed message, containing stuffed BCC2) */
    size_t unstuffed_msg_size;
    byte unstuffed_msg[MSG_INFO_RECEIVER_UNSTUFFED_BUFFER_SIZE];
} state_machine_st;

/**
 * @brief   retrieves the state machine current state
 * @return  state machine current state
 */
state_t getState();

/**
 * @brief   retrieves the state machine message control octet
 * @return  state machine message control octet
 */
byte getMsgCtrl();

/**
 * @brief   passes a byte to the state machine, triggering (or not) state changes
 * @param   msg_byte    byte to be handled by the state machine
 */
void handleMsgByte(byte msg_byte);

/**
 * @brief   retrieves the state machine information buffer (for info messages) - only data, unstuffed, and excludes bcc2
 * @param   msg_size    information buffer size, in bytes
 * @return  information buffer
 */
byte * getInfoMsgBuffer(size_t * msg_size);

/**
 * @brief   resets the state machine
 */
void resetMsgState();

u_ll getSMNumBits();

#endif /* _INFO_STATE_ */