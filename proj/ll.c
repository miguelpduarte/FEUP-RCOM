#include "ll.h"
#include "message.h"
#include "message_defines.h"
#include <stdlib.h>

int llopen(int fd, byte role) {
    byte buf[MSG_SUPERVISION_MSG_SIZE];

    if (role == EMITTER) {
        // Attempt to establish data connection
        int ret = writeSupervisionMessage(fd, MSG_ADDR_EMT, MSG_CTRL_SET);
        if (ret != MSG_SUPERVISION_MSG_SIZE) {
            return WRITE_SUPERVISION_MSG_FAILED;
        }

        // Wait for receiver acknowledgement
        ret = readSupervisionMessage(fd, buf);

        if (ret != MSG_SUPERVISION_MSG_SIZE) {
            return READ_SUPERVISION_MSG_FAILED;
        }
        // Verify correct receiver message type
        else if (buf[MSG_CTRL_IDX] != MSG_CTRL_UA) {
            return INVALID_RECEIVER_ACKNOWLEDGEMENT;
        }
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

    // TODO: Find out what "data connection identifier" means
    return 1;
}

int llwrite(int fd, byte* buffer, size_t length) {

    //Partes a mensagem

    //Transmites bocado a bocado
    //manda 0
    //manda 1
    //manda 2


    return writeInfoMessage(fd, 0, 69, buffer, length);
}