#include "file_handler.h"
#include <stdio.h>
#include "dyn_buffer.h"
#include "utils.h"

#define BUF_LEN 20

int readFile(const char * file_name, dyn_buffer_st* db) {
	FILE * file_ptr;

	file_ptr = fopen(file_name, "rb");
	if (file_ptr == NULL) {
		fprintf(stderr, "File not found.\n");
		return -1;
    }

	// Perform the "copying" itself
	byte buffer[BUF_LEN];
	size_t num_chars;
	while ((num_chars = fread(buffer, sizeof(*buffer), BUF_LEN, file_ptr)) > 0){
		concatBuffer(db, buffer, num_chars);
	}

	if(feof(file_ptr) != 0) {
		//EOF Ocurred
		return 0;
	} else {
		return -2;
	}
}

int writeFile(const char * file_name, dyn_buffer_st * db) {
	FILE * file_ptr;

	file_ptr = fopen(file_name, "wb");
	if (file_ptr == NULL) {
		fprintf(stderr, "File not found.\n");
		return -1;
    }

	int i;
	size_t num_written;
	size_t num_to_write;
	for(i = 0; i < db->length; i += BUF_LEN) {
		num_to_write = MIN(db->length - i, BUF_LEN);
		num_written = fwrite(db->buf + i, sizeof(*(db->buf)), num_to_write, file_ptr);

		if(num_written != num_to_write) {
			return -1;
		}
	}

	return 0;
}