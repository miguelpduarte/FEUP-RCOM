#ifndef _MESSAGE_
#define _MESSAGE_

#include "utils.h"
#include <stdlib.h>
#include "dyn_buffer.h"

typedef struct {
    byte addr;
    byte msg_nr;
    byte bcc2;
} info_message_details_t;

///ERRORS
#define RECEIVER_READ_TIMEOUT       -1
#define RECEIVER_READ_DISC           0

/**
 *  @brief  writes a supervision message to the data connection
 *  @param  fd  data connection file descriptor
 *  @param  msg_addr    message address byte
 *  @param  msg_ctrl    message control byte
 *  @return number of bytes written, negative number if failure
 */
int writeSupervisionMessage(int fd, byte msg_addr, byte msg_ctrl);

/**
 * @brief   writes an information message to the data connection
 * @param   fd  data connection file descriptor
 * @param   info_message_details    structure with info regarding message details (address, number and parity)
 * @param   data    data buffer to be written
 * @param   data_size   data buffer size, in bytes
 * @return  number of bytes written, negative number if failure
 */
int writeInfoMessage(int fd, const info_message_details_t info_message_details, byte * data, size_t data_size);

/**
 *  @brief reads a supervision message from the data connection
 *  @param  fd      data connection file descriptor
 *  @return number of read bytes, negative number on failure
 */
int readSupervisionMessage(int fd);

byte readInfoMsgResponse(int fd, byte msg_nr_S);

int receiverRead(int fd, dyn_buffer_st * dyn_buffer);

int writeSupWithRetry(int fd, byte addr, byte ctrl);

#endif /* _MESSAGE_ */