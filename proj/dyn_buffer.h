#ifndef _DYN_BUFFER_H_
#define _DYN_BUFFER_H_

#define INITIAL_BUFFER_SIZE 10
#define OUT_OF_MEMORY   -42

#include "utils.h"
#include <unistd.h>

/**
 * @brief   dynamically allocated buffer
 */
typedef struct {
    byte* buf;
    size_t buf_size;
    size_t length;
} dyn_buffer_st;

/**
 * @brief   creates a dynamic buffer instance
 * @return  reference to the created dynamic buffer instance
 */
dyn_buffer_st* createBuffer();

/**
 * @brief   concatenates a byte buffer to the dynamic buffer
 * @param   dyn_buffer  dynamic buffer instance reference
 * @param   buf         byte buffer to be concatenated to the dynamic buffer
 * @param   buf_size    byte buffer size
 */
void concatBuffer(dyn_buffer_st* dyn_buffer, byte* buf, const size_t buf_size);

/**
 * @brief   Frees the space allocated for the dynamic buffer instance
 * @param   dyn_buffer  dynamic buffer instance reference
 */
void deleteBuffer(dyn_buffer_st** dyn_buffer);

#endif // _DYN_BUFFER_H_