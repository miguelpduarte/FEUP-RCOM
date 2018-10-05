 //Non-Canonical Input Processing

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include "ll.h"


#define BAUDRATE B38400
#define SERIAL_PORT "/dev/ttyS0"
#define _POSIX_SOURCE 1 // POSIX compliant source

int main(int argc, char** argv) {
    struct termios old_termio, new_termio;
    
    int serial_port_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_port_fd < 0) {
        perror(SERIAL_PORT); 
        exit(-1); 
    }

    if (tcgetattr(serial_port_fd, &old_termio) == -1) { // save current port settings
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&new_termio, sizeof(new_termio));
    new_termio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    new_termio.c_iflag = IGNPAR;
    new_termio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    new_termio.c_lflag = 0;

    new_termio.c_cc[VTIME]    = MSG_WAIT_TIME;
    new_termio.c_cc[VMIN]     = MSG_MIN_CHARS;

    // VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    // leitura do(s) próximo(s) caracter(es)

    tcflush(serial_port_fd, TCIOFLUSH);

    if (tcsetattr(serial_port_fd, TCSANOW, &new_termio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("Waiting for message...\n");

    int ret = llopen(serial_port_fd, RECEIVER);

    if (ret < 0) {
        fprintf(stderr, "llopen() function failed\n");
        exit(-2);
    }

    printf("llopen() successful: %d\n", ret);
  
    sleep(2);
    tcsetattr(serial_port_fd, TCSANOW, &old_termio);
    close(serial_port_fd);

    return 0;
}