#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "dyn_buffer.h"
#include "utils.h"
#include "ll.h"
#include "file_handler.h"
#include "application.h"

int main(int argc, char * argv[]) {
    //1 = emitter, 0 = receiver
    if (argc != 2) {
        printf("Usage: %s <isReceiver?>\n", argv[0]);
        exit(1);
    }

    set_config();
    int fd = get_serial_port_fd();
    int isEmitter = atoi(argv[1]);

    if(isEmitter == EMITTER) {
        sendFile(fd, "foto.jpg");    // TODO: Complete this
    } else {
        retrieveFile(fd);
    }

    // TODO: What to do about this sleep?
    sleep(2);   
    reset_config();

    return 0;
}