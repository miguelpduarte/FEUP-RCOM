#include "dyn_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

dyn_buffer_st* createBuffer() {
    dyn_buffer_st* dyn_buffer = malloc(sizeof(*dyn_buffer));

    if (dyn_buffer == NULL) {
        return NULL;
    }

    dyn_buffer->buf = malloc(INITIAL_BUFFER_SIZE * sizeof(*(dyn_buffer->buf)));
    if (dyn_buffer->buf == NULL) {
        free(dyn_buffer);
        return NULL;
    }

    dyn_buffer->buf_size = INITIAL_BUFFER_SIZE; 
    dyn_buffer->length = 0;   

    return dyn_buffer;  
}

void concatBuffer(dyn_buffer_st* dyn_buffer, byte* buf, const size_t buf_size) {
    if (dyn_buffer->length + buf_size > dyn_buffer->buf_size) {
        size_t necessary_size = (dyn_buffer->length + buf_size) - dyn_buffer->buf_size;

        byte * newbuf = realloc(dyn_buffer->buf, (dyn_buffer->buf_size + necessary_size) * 2);
        if(newbuf == NULL) {
            fprintf(stderr, "Buffer could not be reallocated!\n");
            exit(OUT_OF_MEMORY);
        } else {
            dyn_buffer->buf = newbuf;
            dyn_buffer->buf_size = (dyn_buffer->buf_size + necessary_size) * 2;
        }
    }

    memcpy(dyn_buffer->buf + dyn_buffer->length, buf, buf_size);
    dyn_buffer->length += buf_size;
}

void deleteBuffer(dyn_buffer_st** dyn_buffer) {
    if (dyn_buffer != NULL) {
        free((*dyn_buffer)->buf);
    }

    free(*dyn_buffer);
    *dyn_buffer = NULL;
}
