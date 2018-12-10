#include "file.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int copy_file(int origin_fd, const char* file_name) {
    int destin_fd = open(file_name, O_CREAT | O_WRONLY, FILE_PERMISSIONS);

    if (destin_fd == -1) {
        return FILE_TRANSFER_FAILED;
    }

    ssize_t num_read_bytes;
    ssize_t num_written_bytes;
    char buf[BUF_SIZE];
    while((num_read_bytes = read(origin_fd, buf, BUF_SIZE)) > 0) {
        num_written_bytes = write(destin_fd, buf, num_read_bytes);

        if (num_written_bytes < num_read_bytes) {
            close(destin_fd);
            return FILE_TRANSFER_FAILED;
        }
    }
    
    close(destin_fd);

    if (num_read_bytes == -1) {
        return FILE_TRANSFER_FAILED;
    }

    return 0;
}