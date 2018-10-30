#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

#define EMITTER     0
#define RECEIVER    1

#define MIN(x,y)        (((x) < (y)) ? (x) : (y))
#define MAX(x,y)        (((x) > (y)) ? (x) : (y))
#define GET_SHORT_LSB(x)    ( (x) & 0x00FF)
#define GET_SHORT_MSB(x)    (((x) & 0xFF00) >> 8)
#define BYTE_TO_LSB(x)      ((x) & 0xFF)
#define BYTE_TO_MSB(x)      (((x) & 0xFF) << 8)
#define GET_N_BYTE_OF(x,n)  (((x) >> ((n)*8)) & 0xFF)
#define PROGRESS_BAR_SIZE   30
#define SEPARATOR_CHAR      '='
#define NUM_BACKSPACES      PROGRESS_BAR_SIZE + 9

typedef unsigned char byte;
typedef unsigned short u_short;
typedef unsigned int u_int;

/** Structure regarding data stuffing information */
typedef struct {
    size_t data_bytes_stuffed;      /** Number of data bytes that were processed and stuffed */
    size_t stuffed_buffer_size;     /** Number of bytes that were occupied in the stuffing buffer */
} data_stuffing_t;

/**
 * @brief   computes a message's bcc, by performing multiple XOR operations with the data buffer
 * @param   data    data buffer
 * @param   data_size   data buffer size, in bytes, to be computed
 * @param   data_start_index    buffer index in which the bcc computation must start
 * @return  octet resultant of the bcc operation
 */
byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index);

/**
 * @brief   perfoms stuffing operation on a byte buffer
 * @param   data    data buffer
 * @param   data_size   data buffer size, in bytes, to computed
 * @param   data_start_index    buffer index in which the stuffing operation must start
 * @param   stuffed_buffer  buffer to be filled with the bytes resultant of the stuffing operation
 * @return  structure regarding the information of the stuffing process (data bytes stuffed and stuffed buffer size)
 */
data_stuffing_t stuffData(byte * data, const size_t data_size, const size_t data_start_index, byte * stuffed_buffer);

/**
 * @brief   perfoms unstuffing operation on a byte buffer
 * @param   data    data buffer
 * @param   data_size   data buffer size, in bytes, to computed
 * @param   stuffed_buffer  buffer to be filled with the bytes resultant of the unstuffing operation
 * @return  number of bytes that were unstuffed (size of the unstuffed buffer)
 */
ssize_t unstuffData(byte * data, const size_t data_size, byte * unstuffed_buffer);

/**
 * @brief   stuffs a data byte
 * @param   b   data byte to be stuffed
 * @return  16 bit stuffing result. If no stuffing occured, the MSB will be empty.
 */
u_short stuffByte(byte b);

void printProgressBar(int progress, int total);

void clearProgressBar();

#endif // _UTILS_H_