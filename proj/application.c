#include "application.h"
#include "dyn_buffer.h"
#include <string.h>
#include <stdio.h>

int sendFile(int fd, const char* file_name) {

    return -1;
}

int retrieveFile(dyn_buffer_st* db) {

    return -1;
}

int sendControlPacket(byte ctrl, const char* file_name, size_t file_size) {
    size_t file_name_size = strlen(file_name) + 1;

    byte* packet = malloc(APP_CTRL_PACKET_SIZE(file_name_size) * sizeof(*packet));

    packet[APP_CTRL_IDX] = ctrl;
    packet[APP_FILE_SIZE_T_IDX] = APP_FILE_SIZE_OCTET;
    packet[APP_FILE_SIZE_L_IDX] = sizeof(u_int);
    packet[APP_FILE_SIZE_V_IDX] = GET_N_BYTE_OF(file_size, 3);
    packet[APP_FILE_SIZE_V_IDX + 1] = GET_N_BYTE_OF(file_size, 2);
    packet[APP_FILE_SIZE_V_IDX + 2] = GET_N_BYTE_OF(file_size, 1);
    packet[APP_FILE_SIZE_V_IDX + 3] = GET_N_BYTE_OF(file_size, 0);
    packet[APP_FILE_NAME_T_IDX] = APP_FILE_NAME_OCTET;
    packet[APP_FILE_NAME_L_IDX] = file_name_size;
    
    memcpy(packet + APP_FILE_NAME_V_IDX, file_name, file_name_size);

    return -1;
}