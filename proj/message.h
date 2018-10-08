#ifndef _MESSAGE_
#define _MESSAGE_

#include "message_defines.h"
#include <stdlib.h>

/**
 *  @brief writes a supervision message to the data connection
 *  @param  fd          data connection file descriptor
 *  @param  msg_addr    message address byte
 *  @param  msg_ctrl    message control byte
 *  @return number of bytes written, negative number of failure
 */
int writeSupervisionMessage(int fd, byte msg_addr, byte msg_ctrl);

int writeInfoMessage(int fd, byte msg_addr, byte msg_ctrl, byte * data, size_t data_size);

/**
 *  @brief reads a supervision message from the data connection
 *  @param  fd      data connection file descriptor
 *  @param  buffer  message buffer
 *  @return number of read bytes, negative number on failure
 */
int readSupervisionMessage(int fd, byte* buffer);

#endif /* _MESSAGE_ */