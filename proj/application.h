#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <stdlib.h>
#include "utils.h"
#include "dyn_buffer.h"

#define BIT(n) (0x01<<(n))

// control 
#define APP_CTRL_IDX        0
#define APP_CTRL_DATA       1
#define APP_CTRL_START      2
#define APP_CTRL_END        3

// data packet
#define APP_SEQ_NUM_IDX     1
#define APP_LENGTH_MSB_IDX  2
#define APP_LENGTH_LSB_IDX  3
#define APP_DATA_START_IDX  4
#define APP_SEQ_NUM_SIZE    256

// control packet
#define APP_FILE_SIZE_OCTET             0
#define APP_FILE_NAME_OCTET             1
#define APP_FILE_SIZE_T_IDX             1
#define APP_FILE_SIZE_L_IDX             2
#define APP_FILE_SIZE_V_IDX             3
#define APP_FILE_NAME_T_IDX             7
#define APP_FILE_NAME_L_IDX             8
#define APP_FILE_NAME_V_IDX             9
#define APP_CTRL_PACKET_SIZE(fname_len) sizeof(u_int) + fname_len + 5
#define APP_DATA_PACKET_SIZE(data_len)  data_len + 4


#define LLOPEN_FAILED           -1
#define BUFFER_ALLOC_FAILED     -2
#define FILE_READ_FAILED        -3
#define PACKET_SENDING_FAILED   -4

int sendFile(int fd, const char* file_name);

int retrieveFile(dyn_buffer_st* db);

int sendControlPacket(int fd, byte ctrl, const char* file_name, size_t file_size);

int sendDataPacket(int fd, byte msg_nr, byte* data, u_short data_size);


#endif /* _APPLICATION_H_ */