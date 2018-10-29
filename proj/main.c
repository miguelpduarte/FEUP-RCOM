#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "dyn_buffer.h"
#include "utils.h"
#include "ll.h"
#include "file_handler.h"
#include "application.h"

int main(int argc, char * argv[]) {
    //1 = emitter, 0 = receiver
    if (!(argc == 2 && strcmp(argv[1],"receiver") == 0) &&
        !(argc == 3 && strcmp(argv[1],"emitter") == 0)) {
        fprintf(stderr, "usage: %s [receiver | emitter <file_name>]\n", argv[0]);
        exit(1);
    }

    set_config();

    // Install exit handler to act if anything fails
    atexit(reset_config);

    int fd = get_serial_port_fd();

    if(strcmp(argv[1],"emitter") == 0) {
        sendFile(fd, argv[2]);    // TODO: Complete this
    } else {
        retrieveFile(fd);
    }

    return 0;
}