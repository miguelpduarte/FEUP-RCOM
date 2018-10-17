#ifndef _MESSAGE_
#define _MESSAGE_

#include "utils.h"
#include <stdlib.h>

typedef struct {
    byte addr;
    byte msg_nr;
    byte bcc2;
} info_message_details_t;

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
 *  @param  buffer  message buffer
 *  @return number of read bytes, negative number on failure
 */
int readSupervisionMessage(int fd, byte* buffer);

byte readInfoMsgResponse(int fd, byte msg_nr_S);

#endif /* _MESSAGE_ */