#ifndef _LL_
#define _LL_

#include "message_defines.h"
#include "utils.h"
#include "dyn_buffer.h"
#include <stdlib.h>

#define EMITTER     0
#define RECEIVER    1

// Errors
#define WRITE_SUPERVISION_MSG_FAILED        -1
#define INVALID_COMMUNICATION_ROLE          -2
#define ESTABLISH_DATA_CONNECTION_FAILED    -3
#define LLWRITE_FAILED                      -4

//Successes
#define MSG_IDENTIFIER                      1

/**
 * @brief   opens data connection
 * @param   fd      data connection file descriptor
 * @param   role    communication role, TRASMITTER(0) | RECEIVER(1)
 * @return  data connection identifier on success, negative number on failure
*/
int llopen(int fd, byte role);

/**
 * @brief   closes data connection
 * @param   fd  data connection file descriptor
 * @return  0 on success, negative number on failure
*/
int llclose(int fd);

/**
 * @brief   writes a message to the data connection
 * @param   fd      data connection file descriptor
 * @param   buffer  buffer message
 * @param   length  buffer message number of bytes
 * @return  number of written bytes, negative number on failure
*/
int llwrite(int fd, byte* buffer, const size_t length);


/**
 * @brief   reads a message from the data connection
 * @param   fd      data connection file descriptor
 * @param   buffer  read buffer message
 * @return  number of read bytes, negative number on failure
*/
int llread(int fd, dyn_buffer_st * dyn_buffer);

#endif /* _LL_ */