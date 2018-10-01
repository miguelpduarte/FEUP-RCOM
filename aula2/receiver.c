/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include "message.h"

#define BAUDRATE B38400
#define SERIAL_PORT "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char** argv) {
    struct termios old_termio, new_termio;
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
    
    int serial_port_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY );
    if (serial_port_fd < 0) {
        perror(SERIAL_PORT); 
        exit(-1); 
    }

    if ( tcgetattr(serial_port_fd, &old_termio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&new_termio, sizeof(new_termio));
    new_termio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    new_termio.c_iflag = IGNPAR;
    new_termio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    new_termio.c_lflag = 0;

    new_termio.c_cc[VTIME]    = 30;   /* inter-character timer set for 3 seconds */
    new_termio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */

    tcflush(serial_port_fd, TCIOFLUSH);

    if ( tcsetattr(serial_port_fd, TCSANOW, &new_termio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("Waiting for message...\n");

    char byte, message[4096];
    size_t num_bytes_read = 0;
    ssize_t res;
    while (1) {
      res = read(serial_port_fd, &byte, 1);

      if (res == -1) {
        fprintf(stderr, "Error in reading from " SERIAL_PORT "\n");
        exit(-2);
      }

      message[num_bytes_read++] = byte;

      if (byte == '\0') {
        break;
      }
    }

    printf("\n%zd bytes read from the serial port\n", num_bytes_read);
    printf("Message: %s\n", message);

    printf("\nSending message ...\n");

    res = write(serial_port_fd, message, strlen(message)+1);
    if (res == -1) {
      fprintf(stderr, "Error in writing to " SERIAL_PORT "\n");
      exit(-3);
    } else if (res != strlen(message) + 1) {
      fprintf(stderr, "Error: could not send the whole message!!!\n");
      exit(-4);
    }

    printf("Message sent.\n");
  
    sleep(2);
    tcsetattr(serial_port_fd, TCSANOW, &old_termio);
    close(serial_port_fd);

    return 0;
}
