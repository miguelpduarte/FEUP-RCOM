#ifndef _MESSAGE_
#define _MESSAGE_

#include "message_defines.h"
#include "state.h"

/**
 *  @brief writes a supervision message to the data connection
 *  @param  fd          data connection file descriptor
 *  @param  msg_addr    message address byte
 *  @param  msg_ctrl    message control byte
 *  @return number of bytes written, negative number of failure
 */
int writeSupervisionMessage(int fd, byte msg_addr, byte msg_ctrl);

/**
 *  @brief reads a supervision message from the data connection
 *  @param  fd      data connection file descriptor
 *  @param  buffer  message buffer
 *  @return number of read bytes, negative number on failure
 */
int readSupervisionMessage(int fd, byte* buffer);

/**
 *  @brief reads a byte from the data connection, placing it in the msg_byte parameter
 *  @param  fd          data connection file descriptor
 *  @param  msg_byte    message byte
 *  @return positive number on success, negative number on failure
 */
int readMessageByte(int fd, byte* msg_byte);

#endif /* _MESSAGE_ */