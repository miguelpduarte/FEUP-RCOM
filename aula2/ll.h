#ifndef _LL_
#define _LL_

#include "message_defines.h"

#define TRANSMITTER 0
#define RECEIVER    1

// Errors
#define WRITE_SUPERVISION_MSG_FAILED        -1
#define READ_SUPERVISION_MSG_FAILED         -2
#define INVALID_RECEIVER_ACKNOWLEDGEMENT    -3

/**
 * @brief   opens data connection connection
 * @param   fd      data connection file descriptor
 * @param   role    communication role, TRASMITTER(0) | RECEIVER(1)
 * @return  data connection identifier on success, negative number on failure
*/
int llopen(int fd, byte role);

/**
 * @brief   closes data connection
 * @param   fd  data connection file descriptor
 * @return  positive number on success, negative number on failure
*/
int llclose(int fd);

/**
 * @brief   writes a message to the data connection
 * @param   fd      data connection file descriptor
 * @param   buffer  buffer message
 * @param   length  buffer message number of bytes
 * @return  number of written bytes, negative number on failure
*/
int llwrite(int fd, byte* buffer, int length);


/**
 * @brief   reads a message from the data connection
 * @param   fd      data connection file descriptor
 * @param   buffer  read buffer message
 * @return  number of read bytes, negative number on failure
*/
int llread(int fd, byte* buffer);

#endif /* _LL_ */