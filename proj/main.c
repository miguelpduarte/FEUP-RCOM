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

    printf("good params\n");
    return 0;

    set_config();
    int fd = get_serial_port_fd();

    if(strcmp(argv[1],"emitter") == 0) {
        sendFile(fd, argv[2]);    // TODO: Complete this
    } else {
        retrieveFile(fd);
    }

    // TODO: What to do about this sleep?
    sleep(2);   
    reset_config();

    return 0;
}