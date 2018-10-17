#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

#define MIN(x,y)        (((x) < (y)) ? (x) : (y))
#define MAX(x,y)        (((x) > (y)) ? (x) : (y))
#define SHORT_LSB(x)    ( (x) & 0x00FF)
#define SHORT_MSB(x)    (((x) & 0xFF00) >> 8)

typedef unsigned char byte;

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
size_t unstuffData(byte * data, const size_t data_size, byte * unstuffed_buffer);

/**
 * @brief   stuffs a data byte
 * @param   b   data byte to be stuffed
 * @return  16 bit stuffing result. If no stuffing occured, the MSB will be empty.
 */
unsigned short stuffByte(byte b);

#endif // _UTILS_H_