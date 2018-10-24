#include "application.h"
#include "dyn_buffer.h"
#include "ll.h"
#include "file_handler.h"
#include <string.h>
#include <stdio.h>

int sendFile(int fd, const char* file_name) {
    // Read file's content
    dyn_buffer_st* db = createBuffer();
    if (db == NULL) {
        return BUFFER_ALLOC_FAILED;
    }

    if(readFile(file_name, db) != 0) {
        return FILE_READ_FAILED;
    }

    // Establish communication with receiver
    int ll_ret = llopen(fd, EMITTER);
    if (ll_ret < 0) {
        return LLOPEN_FAILED;   
    }

    // Send file info data (name and size)
    if (sendControlPacket(fd, APP_CTRL_START, file_name, db->length) != 0) {
        // TODO: WHat to do here?
        return PACKET_SENDING_FAILED;
    }

    // Send file content
    // TODO

    // Send file end packet (with name and size also)
    if (sendControlPacket(fd, APP_CTRL_END, file_name, db->length) != 0) {
        // TODO: WHat to do here?
        return PACKET_SENDING_FAILED;
    }

    // Close data connection: TODO Should the success of llclose() be verified? I mean, se um close falha, falhou, o programa vai terminar e vai ':D
    llclose(fd);    
    deleteBuffer(&db);

    return 0;
}

int retrieveFile(dyn_buffer_st* db) {

    return -1;
}

int sendControlPacket(int fd, byte ctrl, const char* file_name, size_t file_size) {
    size_t file_name_size = strlen(file_name) + 1;

    // Build Packet
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

    // Send Packet
    int ll_ret = llwrite(fd, packet, APP_CTRL_PACKET_SIZE(file_name_size) * sizeof(*packet));
    free(packet);

    return ll_ret;
}

int sendDataPacket(int fd, byte msg_nr, byte* data, u_short data_size) {
    byte* packet = malloc(APP_DATA_PACKET_SIZE(data_size) * sizeof(*packet));

    packet[APP_CTRL_IDX] = APP_CTRL_DATA;
    packet[APP_SEQ_NUM_IDX] = msg_nr % APP_SEQ_NUM_SIZE;    // TODO: Fica este MOD aqui? Tecnicamente por ser 'byte' já tá mod 256
    packet[APP_LENGTH_MSB_IDX] = GET_SHORT_MSB(data_size);
    packet[APP_LENGTH_LSB_IDX] = GET_SHORT_LSB(data_size);
    
    memcpy(packet + APP_DATA_START_IDX, data, data_size);

    int ll_ret = llwrite(fd, data, APP_DATA_PACKET_SIZE(data_size) * sizeof(*packet));
    free(packet);

    return ll_ret;
}