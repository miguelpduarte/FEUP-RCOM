#include "application.h"
#include "dyn_buffer.h"
#include "ll.h"
#include "message.h"
#include "file_handler.h"
#include <string.h>
#include <stdio.h>
#include "chrono.h"

static size_t num_packets;
static size_t num_file_bits;

/**
 * @brief   sends a control packet to the serial port data connection
 * @param   fd          data connection file descriptor
 * @param   ctrl        packet control octet
 * @param   file_name   file name
 * @param   file_size   file size
 * @return  0 on success, non-zero otherwise
 */
static int sendControlPacket(int fd, byte ctrl, const char* file_name, size_t file_size);

/**
 * @brief   sends a control packet to the serial port data connection
 * @param   fd          data connection file descriptor
 * @param   msg_nr      message number
 * @param   data        data buffer
 * @param   data_size   data buffer size
 * @return  0 on success, non-zero otherwise
 */
static int sendDataPacket(int fd, byte msg_nr, byte* data, u_short data_size);

/**
 * @brief   interpretes the packets received to verify file validity
 * @param   dynamic buffer reference, containing all the packets received
 * @return  0 on success, non-zero otherwise
 */
static int interpretPackets(dyn_buffer_st * db);

int sendFile(int fd, const char* file_name) {
    // Read file's content
    dyn_buffer_st* db = createBuffer();
    if (db == NULL) {
        return BUFFER_ALLOC_FAILED;
    }

    printf("Reading from file.\n");

    if(readFile(file_name, db) != 0) {
        deleteBuffer(&db);
        return FILE_READ_FAILED;
    } else if (db->length == 0) {
        deleteBuffer(&db);
        return EMPTY_FILE;
    }

    printf("Establishing connection.\n");

    // Establish communication with receiver
    if (llopen(fd, EMITTER) < 0) {
        fprintf(stderr, "\nError: Connection timed out.\n");
        deleteBuffer(&db);
        return LLOPEN_FAILED;   
    }

    printf("Connection established. Sending file.\n\n");

    start_clock();
    
    // Send file info data (name and size)
    if (sendControlPacket(fd, APP_CTRL_START, file_name, db->length) != 0) {
        deleteBuffer(&db);
        return PACKET_SENDING_FAILED;
    }

    num_packets++;

    // Send file content
    int i;
    int ret;
    u_short data_size = 0;
    byte msg_nr = 0;
    for (i = 0; i < db->length; i += data_size) {
        printProgressBar(i, db->length);

        data_size = MIN(db->length - i, APP_DATA_PACKET_MAX_SIZE);
        ret = sendDataPacket(fd, msg_nr, db->buf + i, data_size);

        if(ret != 0) {
            deleteBuffer(&db);
            return PACKET_SENDING_FAILED;
        }
        
        num_packets++;

        msg_nr++;
        clearProgressBar();
    }

    printProgressBar(1, 1);

    // Send file end packet (with name and size also)
    if (sendControlPacket(fd, APP_CTRL_END, file_name, db->length) != 0) {
        // TODO: WHat to do here?
        deleteBuffer(&db);
        return PACKET_SENDING_FAILED;
    }

    stop_clock();
    print_clock_diff();

    num_packets++;
    printf("\n\nFile sent successfully, closing connection.\n");

    // Close data connection
    llclose(fd);    
    deleteBuffer(&db);

    printf("Connection closed.\n\nDone.\n");

    return 0;
}

int retrieveFile(int fd) {
    dyn_buffer_st* db = createBuffer();
    if (db == NULL) {
        fprintf(stderr, "Error: Not enough memory to allocate reading buffer.\n");
        return BUFFER_ALLOC_FAILED;
    }

    printf("Waiting for connection.\n");
    // Establish communication with receiver
    if (llopen(fd, RECEIVER) < 0) {
        deleteBuffer(&db);
        return LLOPEN_FAILED;
    }

    printf("Connection established, reading data.\n");

    // Read file
    if (llread(fd, db) != 0) {
        deleteBuffer(&db);
        return FILE_READING_FAILED;
    }

    printf("\nData read successfully!\n");

    //Interpreting packets and writing to file
    if(interpretPackets(db) != 0) {
        deleteBuffer(&db);
        fprintf(stderr, "Error interpreting data packets!\n");
        return PACKET_INTERPRETE_FAIL;
    }

    deleteBuffer(&db);
    printf("File created successfully.\n");

    printf("\n\nDone.\n");
    return 0;
}

static int sendControlPacket(int fd, byte ctrl, const char* file_name, size_t file_size) {
    size_t file_name_size = strlen(file_name) + 1;
    if(file_name_size > 255) {
        fprintf(stderr, "Due to system restrictions, the file name cannot be larger than 255 characters\n");
        return FILE_NAME_SIZE_OVERFLOW;
    }

    // Build Packet
    byte* packet = malloc(APP_CTRL_PACKET_SIZE(file_name_size) * sizeof(*packet));
    if (packet == NULL) {
        return OUT_OF_MEMORY;
    }

    packet[APP_CTRL_IDX] = ctrl;
    packet[APP_FILE_SIZE_T_IDX] = APP_FILE_SIZE_OCTET;
    packet[APP_FILE_SIZE_L_IDX] = sizeof(u_int);
    packet[APP_FILE_SIZE_V_IDX] = GET_N_BYTE_OF(file_size, 3);
    packet[APP_FILE_SIZE_V_IDX + 1] = GET_N_BYTE_OF(file_size, 2);
    packet[APP_FILE_SIZE_V_IDX + 2] = GET_N_BYTE_OF(file_size, 1);
    packet[APP_FILE_SIZE_V_IDX + 3] = GET_N_BYTE_OF(file_size, 0);
    packet[APP_FILE_NAME_T_IDX] = APP_FILE_NAME_OCTET;
    //Because this is an octet, the file name cannot be larger than 255 characters
    packet[APP_FILE_NAME_L_IDX] = (byte) file_name_size;
    
    memcpy(packet + APP_FILE_NAME_V_IDX, file_name, file_name_size);

    // Send Packet
    int ll_ret = llwrite(fd, packet, APP_CTRL_PACKET_SIZE(file_name_size) * sizeof(*packet));
    free(packet);

    return ll_ret;
}

static int sendDataPacket(int fd, byte msg_nr, byte* data, u_short data_size) {
    // Build Packet
    byte* packet = malloc(APP_DATA_PACKET_SIZE(data_size) * sizeof(*packet));
    if (packet == NULL) {
        return OUT_OF_MEMORY;
    }

    packet[APP_CTRL_IDX] = APP_CTRL_DATA;
    packet[APP_SEQ_NUM_IDX] = msg_nr % APP_SEQ_NUM_SIZE;
    packet[APP_LENGTH_MSB_IDX] = GET_SHORT_MSB(data_size);
    packet[APP_LENGTH_LSB_IDX] = GET_SHORT_LSB(data_size);
    
    memcpy(packet + APP_DATA_START_IDX, data, data_size);

    // Send Packet
    int ll_ret = llwrite(fd, packet, APP_DATA_PACKET_SIZE(data_size) * sizeof(*packet));
    free(packet);

    return ll_ret;
}

static int interpretPackets(dyn_buffer_st * db) {
    //Interpret initial control packet
    if (db->buf[APP_CTRL_IDX] != APP_CTRL_START) {
        return BAD_START_PACKET;
    }

    if (db->buf[APP_FILE_SIZE_T_IDX] != APP_FILE_SIZE_OCTET) {
        return BAD_START_PACKET;
    }

    dyn_buffer_st * file_content = createBuffer();
    if (file_content == NULL) {
        return OUT_OF_MEMORY;
    }

    u_int file_size = 0;
    memcpy(&file_size, db->buf + APP_FILE_SIZE_V_IDX, db->buf[APP_FILE_SIZE_L_IDX]);

    if (db->buf[APP_FILE_NAME_T_IDX] != APP_FILE_NAME_OCTET) {
        deleteBuffer(&file_content);
        return BAD_START_PACKET;
    }

    byte file_name_size = db->buf[APP_FILE_NAME_L_IDX];

    char * file_name = malloc(file_name_size * sizeof(*file_name));
    if(file_name == NULL) {
        deleteBuffer(&file_content);
        return OUT_OF_MEMORY;
    }
    memcpy(file_name, db->buf + APP_FILE_NAME_V_IDX, file_name_size);

    num_packets++;

    size_t i = APP_FILE_NAME_V_IDX + file_name_size;
    byte curr_ctrl;
    byte msg_nr;
    u_short packet_size;

    while(1) {       
        // Read ctrl byte and check for data termination
        curr_ctrl = db->buf[i];

        if (curr_ctrl == APP_CTRL_END) {
            break;
        } else if (curr_ctrl != APP_CTRL_DATA) {
            free(file_name);
            deleteBuffer(&file_content);
            return BAD_DATA_PACKET;
        }

        if (msg_nr++ != db->buf[i + 1]) {
            free(file_name);
            deleteBuffer(&file_content);
            return BAD_DATA_PACKET;
        }

        packet_size = BYTE_TO_MSB(db->buf[i + 2]) + BYTE_TO_LSB(db->buf[i + 3]);

        concatBuffer(file_content, db->buf + i + 4, packet_size);
        num_packets++;

        i += APP_DATA_PACKET_SIZE(packet_size);
    }

    //Interpret final control packet
    if (memcmp(db->buf + 1, db->buf + i + 1, file_name_size + 8) != 0) {
        return BAD_FINAL_PACKET;
    }

    num_packets++;

    printf("Creating '%s'\n", file_name);

    int write_file_ret = writeFile(file_name, file_content);
    num_file_bits = file_content->length * 8;

    free(file_name);
    deleteBuffer(&file_content);

    return write_file_ret;
}

void printTransferInfo(int isReceiver) {
    printf("\nNumber of Packets %s: %zu\n", isReceiver ? "received" : "sent", num_packets);

    if (isReceiver) {
        printf("\nFile Size (bits): %zu\n", num_file_bits);
        printf("Number of bits received: %llu\n", getNumBits());
        printf("'Extra' bits: %llu\n", getNumBits() - num_file_bits);
    }

    printf("\nNumber of RRs %s: %zu\n", isReceiver ? "received" : "sent", getNumRRs());
    printf("Number of REJs %s: %zu\n", isReceiver ? "received" : "sent", getNumRejs());
}