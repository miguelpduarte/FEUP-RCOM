#include <stdio.h>
#include <unistd.h>
#include "config.h"
#include "dyn_buffer.h"

#include "ll.h"

int main(int argc, char * argv[]) {    
    //1 = emitter, 0 = receiver
    if (argc != 2) {
        printf("Usage: %s <isEmitter?>\n", argv[0]);
        exit(1);
    }

    set_config();

    int serial_port_fd = get_serial_port_fd();

    int isEmitter = atoi(argv[1]);

    int ll_ret;

    if(isEmitter == EMITTER) {
        ll_ret = llopen(serial_port_fd, EMITTER);

        if (ll_ret < 0) {
            fprintf(stderr, "emitter: llopen() function failed\n");
            exit(-3);
        } else {
            printf("emitter: llopen() successful: %d.\n", ll_ret);
        }

        const int sending_buf_size = 1097;
        byte sending_buf[sending_buf_size];

        int i;
        for(i = 0; i < sending_buf_size; ++i) {
            sending_buf[i] = (i % 10);
        }

        llwrite(serial_port_fd, sending_buf, sending_buf_size);
        llclose(serial_port_fd);
    } else {
        ll_ret = llopen(serial_port_fd, RECEIVER);

        if (ll_ret < 0) {
            fprintf(stderr, "receiver: llopen() function failed\n");
            exit(-3);
        } else {
            printf("receiver: llopen() successful: %d.\n", ll_ret);
        }

        dyn_buffer_st * dyn_buffer = createBuffer();
        if(dyn_buffer == NULL) {
            fprintf(stderr, "Allocation of dynamic buffer failed\n");
            exit(-4);
        }

        llread(serial_port_fd, dyn_buffer);

        printf("Received the following message:\n");
        int i;
        for(i = 0; i < dyn_buffer->length; ++i) {
            printf("%c", dyn_buffer->buf[i]);
        }

        deleteBuffer(&dyn_buffer);
    }

    // TODO: What to do about this sleep?
    sleep(2);   
    reset_config();

    return 0;    
}