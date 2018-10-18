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
#define WRITE_SUP_MSG_FAILED        -2
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
 *  @brief  reads a supervision message from the data connection
 *  @param  fd      data connection file descriptor
 *  @return number of read bytes, negative number on failure
 */
int readSupervisionMessage(int fd);

/**
 * @brief   reads a response to an information message
 * @param   fd  data connection file descriptor
 * @param   msg_nr_S    message number funcion value, S(n)
 * @return  new message number S(n) based on response
 */
byte readInfoMsgResponse(int fd, byte msg_nr_S);

/**
 * @brief   receiver reading loop
 * @param   fd  data connection file descriptor
 * @param   dyn_buffer  dynamic buffer reference, to be filled with the read message
 * @return  RECEIVER_READ_DISC on success, RECEIVER_READ_TIMEOUT on timeout
 */
int receiverRead(int fd, dyn_buffer_st * dyn_buffer);

/**
 * @brief   writes a supervision message and retries on failure
 * @param   fd  data connection file descriptor
 * @param   addr    message address byte
 * @param   ctrl    message control byte
 * @return  returns 0 on success, WRITE_SUP_MSG_FAILED otherwise
 */
int writeSupWithRetry(int fd, byte addr, byte ctrl);

#endif /* _MESSAGE_ */