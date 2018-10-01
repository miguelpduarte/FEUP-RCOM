/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "message_defines.h"
#include "message.h"

#define BAUDRATE B38400
#define SERIAL_PORT "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s", argv[0]);
        exit(1);
    }

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    int serial_port_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_port_fd < 0) {
        perror(SERIAL_PORT);
        exit(-1);
    }

    struct termios old_termio, new_termio;

    if (tcgetattr(serial_port_fd, &old_termio) == -1) {
        // save current port settings
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&new_termio, sizeof(new_termio));
    new_termio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    new_termio.c_iflag = IGNPAR;
    new_termio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    new_termio.c_lflag = 0;

    new_termio.c_cc[VTIME] = MSG_WAIT_TIME;
    new_termio.c_cc[VMIN] = MSG_MIN_CHARS;

    tcflush(serial_port_fd, TCIOFLUSH);

    if (tcsetattr(serial_port_fd, TCSANOW, &new_termio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    ssize_t n_bytes;

    byte msg_buf[MSG_SUPERVISION_MSG_SIZE];
    buildSupervisionMessage(MSG_ADDR_EMT, MSG_CTRL_SET, msg_buf);

    n_bytes = write(serial_port_fd, msg_buf, MSG_SUPERVISION_MSG_SIZE);
    if (n_bytes == -1) {
        fprintf(stderr, "Failed to write in serial port");
        exit(-3);
    }

    printf("%zd bytes written to serial port\n", n_bytes);
    //Waiting to ensure that the data is sent before resetting SP configurations
    
    // char message[4096];
    // char byte;
    // n = 0;

    // while(1){
    //     n_bytes = read(serial_port_fd, &byte, 1);

    //     if(n_bytes == -1){
    //         fprintf(stderr, "Error reading return message");
    //         exit(-4);
    //     }

    //     message[n++] = byte;

    //     if(byte == '\0'){
    //         break;
    //     }
    // }

    // printf("\nWaiting message...\n");
    // printf("Message returned: %s\n", message);
    
    sleep(2);

    if (tcsetattr(serial_port_fd, TCSANOW, &old_termio) == -1) {
        perror("tcsetattr reset");
        exit(-1);
    }

    close(serial_port_fd);
    return 0;
}
