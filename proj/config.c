#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "ll.h"
#include "message_defines.h" //TODO: REMOVE

#define BAUDRATE B38400
#define SERIAL_PORT "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

static struct termios old_termio;
static int serial_port_fd;

void set_config() {
    serial_port_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_port_fd < 0) {
        perror(SERIAL_PORT);
        exit(-1);
    }

    struct termios new_termio;

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
        exit(-2);
    }
}

void reset_config() {
    sleep(2);  

    if (tcsetattr(serial_port_fd, TCSANOW, &old_termio) == -1) {
        perror("tcsetattr reset");
        exit(-4);
    }

    close(serial_port_fd);
}

int get_serial_port_fd() {
    return serial_port_fd;
}