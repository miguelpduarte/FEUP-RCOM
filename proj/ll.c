#include "ll.h"
#include "message.h"
#include "message_defines.h"
#include <stdlib.h>
#include <unistd.h>

static byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index);

int llopen(int fd, byte role) {
    byte buf[MSG_SUPERVISION_MSG_SIZE];

    if (role == EMITTER) {
        int num_tries, ret;
        for (num_tries = 0; num_tries < MSG_NUM_READ_TRIES; num_tries++) {
            ret = writeSupervisionMessage(fd, MSG_ADDR_EMT, MSG_CTRL_SET);

            // If the whole message could not be written, retry to write it
            if (ret != MSG_SUPERVISION_MSG_SIZE) {
                continue;
            }

            // Wait for receiver acknowledgement
            ret = readSupervisionMessage(fd, buf);

            // Verify correct receiver message type
            if (ret != MSG_SUPERVISION_MSG_SIZE || buf[MSG_CTRL_IDX] != MSG_CTRL_UA) {
                continue;
            }
            else {
                // TODO: Find out what to do with connection id
                return 1;
            }
        }

        return ESTABLISH_DATA_CONNECTION_FAILED;        
    }
    else if (role == RECEIVER) {
        // Wait until transmitter tries to start communication
        int ret;
        do {
            ret = readSupervisionMessage(fd, buf);

            if (ret != MSG_SUPERVISION_MSG_SIZE) {
                continue;
            }
        } while (buf[MSG_CTRL_IDX] != MSG_CTRL_SET);

        ret = writeSupervisionMessage(fd, MSG_ADDR_REC, MSG_CTRL_UA);
        if (ret != MSG_SUPERVISION_MSG_SIZE) {
            return WRITE_SUPERVISION_MSG_FAILED;
        }        
    }
    else {
        return INVALID_COMMUNICATION_ROLE;
    }

    // TODO: Find out what to do with connection id
    return 1;
}

int llwrite(int fd, byte* buffer, size_t length) {
    size_t num_messages = (length / MSG_PART_MAX_SIZE) + ((length % MSG_PART_MAX_SIZE) != 0);
    size_t i, num_bytes_written = 0;
    byte bcc2;

    size_t j;

    data_stuffing_t ds;
    byte stuffed_data_buffer[MSG_STUFFING_BUFFER_SIZE];
    for (i = 0; i < num_messages; i++) {        
        ds = stuffData(buffer, length, num_bytes_written, stuffed_data_buffer);
        bcc2 = calcBcc2(buffer, ds.data_bytes_stuffed, i*MSG_PART_MAX_SIZE);
        num_bytes_written += ds.data_bytes_stuffed;
        /* 
            Estas operações estão a ser feitas corretamente, já testei com vários valores
            Agora deve ser feito o envio da mensagem em si e reenvio / prosseguir
        */


        printf("\nIteration #%d: bytes [%d , %d]\n[", i, i*MSG_PART_MAX_SIZE, num_bytes_written);
        for (j=0 ; j<ds.stuffed_buffer_size ; j++) {
            printf("%d ", stuffed_data_buffer[j]);
        }
        printf("]\n");
    }

    printf("\nEND\n");
    
    //Partes a mensagem

    //Transmites bocado a bocado
    //manda 0:
        //fazer paridade da informação -> Bcc2
        //fazer stuffing
        //enviar para o writeInfoMessage
    //manda 1
        //fazer paridade da informação -> Bcc2
        //fazer stuffing
        //enviar para o writeInfoMessage
    //manda 2

    return -1;
}

static byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index) {
    byte bcc2 = data[data_start_index];
    size_t i = data_start_index + 1;

    for(; i < data_start_index + data_size; ++i) {
        bcc2 ^= data[i];
    }

    return bcc2;
}

data_stuffing_t stuffData(byte * data, const size_t data_size, const size_t data_start_index, byte * stuffed_buffer) {
    size_t data_index = data_start_index, stuffed_buffer_index = 0;
    size_t num_loops = MIN(MSG_PART_MAX_SIZE, (ssize_t) (data_size - data_start_index));

    for (; data_index < data_start_index + num_loops; data_index++) {
        if (data[data_index] == MSG_FLAG) {
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            stuffed_buffer[stuffed_buffer_index++] = MSG_FLAG_STUFFING_BYTE;
        } else if (data[data_index] == MSG_ESCAPE_BYTE) {
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            stuffed_buffer[stuffed_buffer_index++] = MSG_ESCAPE_STUFFING_BYTE;
        } else {
            stuffed_buffer[stuffed_buffer_index++] = data[data_index];
        }
    }

    data_stuffing_t ds;
    ds.data_bytes_stuffed = num_loops;
    ds.stuffed_buffer_size = stuffed_buffer_index;

    return ds;
}

data_unstuffing_t unstuffData(byte * data, const size_t data_size, const size_t data_start_index, byte * unstuffed_buffer) {
    size_t data_index = data_start_index, unstuffed_buffer_index = 0;
    size_t num_loops = MIN(MSG_PART_MAX_SIZE, (ssize_t) (data_size - data_start_index));

    while(unstuffed_buffer_index < MSG_PART_MAX_SIZE && data_index < data_size) {
        if (data[data_index] == MSG_ESCAPE_BYTE) {
            data_index++;
            if (data[data_index] == MSG_FLAG_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_FLAG;
            } else if (data[data_index] == MSG_ESCAPE_STUFFING_BYTE) {
                unstuffed_buffer[unstuffed_buffer_index++] = MSG_ESCAPE_BYTE;
            } else {
                // TODO: What to do in this case? This is not supposed to happen if everything goes well according to the protocol :/
                exit(-1);
            }
        }
        else {
            unstuffed_buffer[unstuffed_buffer_index++] = data[data_index];
        }

        data_index++;   // TODO: this could be a for loop, but the thought process is much easier and more coherent with a while loop IMO
    }

    data_unstuffing_t dus;
    dus.data_bytes_unstuffed = data_index - data_start_index;
    dus.unstuffed_buffer_size = unstuffed_buffer_index;

    return dus;
}
