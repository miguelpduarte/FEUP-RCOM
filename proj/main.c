#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "dyn_buffer.h"
#include "utils.h"
#include "file_handler.h"
#include "application.h"

#define INVALID_ARGUMENTS       1
#define FILE_SENDING_FAILED     2
#define FILE_RECEIVING_FAILED   3

int main(int argc, char * argv[]) {
    //1 = emitter, 0 = receiver
    if (!(argc == 2 && strcmp(argv[1],"receiver") == 0) &&
        !(argc == 3 && strcmp(argv[1],"emitter") == 0)) {
        fprintf(stderr, "usage: %s [receiver | emitter <file_name>]\n", argv[0]);
        exit(INVALID_ARGUMENTS);
    }

    set_config();

    // Install exit handler to act if anything fails
    atexit(reset_config);

    int fd = get_serial_port_fd();

    int t_measure1, t_measure2;
    struct timespec start_time, end_time;

    t_measure1 = clock_gettime(CLOCK_REALTIME, &start_time);

    if(strcmp(argv[1],"emitter") == 0) {
        if (sendFile(fd, argv[2]) != 0) {
            fprintf(stderr, "\nFile sending failed.\n");
            return FILE_SENDING_FAILED;
        }
        printTransferInfo(EMITTER);
    } else {
        if (retrieveFile(fd) != 0) {
            fprintf(stderr, "\nFile receiving failed.\n");
            return FILE_RECEIVING_FAILED;
        }
        printTransferInfo(RECEIVER);
    }

    t_measure2 = clock_gettime(CLOCK_REALTIME, &end_time);

    if (t_measure1 == -1 || t_measure2 == -1) {
        fprintf(stderr, "Failed to measure elapsed time.");
    } else {
        double elapsed_time = ( end_time.tv_sec - start_time.tv_sec )
                            + ( end_time.tv_nsec - start_time.tv_nsec )
                            / 1e9;
        printf("Elapsed time: %lfs\n", elapsed_time);
    }

    return 0;
}