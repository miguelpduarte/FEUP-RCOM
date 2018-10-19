#include "file_reader.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "dyn_buffer.h"

#define BUF_LEN 20

int readFile(dyn_buffer_st* db) {
	int fd = open("lorem.txt" , O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "File not found.\n");
		return -1;
    }

	// Perform the "copying" itself
	unsigned char buffer[BUF_LEN];
	size_t num_chars;
	while ((num_chars = read(fd, buffer, BUF_LEN)) > 0){
        if (num_chars < 0) {
            return -2;
        }
        
		concatBuffer(db, buffer, num_chars);
	}

    return 0;
}