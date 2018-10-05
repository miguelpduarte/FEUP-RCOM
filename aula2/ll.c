#include "ll.h"
#include "message.h"
#include "message_defines.h"

int llopen(int fd, byte role) {
    byte msg_addr = (role == TRANSMITTER) ? MSG_ADDR_EMT : MSG_ADDR_REC;

    // Attempt to establish data connection
    int ret = writeSupervisionMessage(fd, msg_addr, MSG_CTRL_SET);
    if (ret != MSG_SUPERVISION_MSG_SIZE) {
        return WRITE_SUPERVISION_MSG_FAILED;
    }

    // Wait for receiver acknowledgement
    byte buf[MSG_SUPERVISION_MSG_SIZE];
    ret = readSupervisionMessage(fd, buf);

    if (ret != MSG_SUPERVISION_MSG_SIZE) {
        return READ_SUPERVISION_MSG_FAILED;
    }
    // Verify correct receiver message type
    else if (buf[MSG_CTRL_IDX] != MSG_CTRL_UA) {
        return INVALID_RECEIVER_ACKNOWLEDGEMENT;
    }
    // TODO: Find out what "data connection identifier" means
    else {
        return 1;
    }
}