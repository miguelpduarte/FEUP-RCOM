#ifndef _FILE_HANDLER_H_
#define _FILE_HANDLER_H_

#include "dyn_buffer.h"

#define FILE_NOT_FOUND      1
#define FILE_READ_FAILED    2
#define FILE_WRITE_FAILED   3

/**
 * @brief   reads the content of a file, placing it in program memory
 * @param   file_name   file name
 * @param   db  dynamic buffer reference
 * @return  0 on success, non-zero otherwise
 */
int readFile(const char * file_name, dyn_buffer_st* db);

/**
 * @brief   writes the content of a buffer to a file
 * @param   file_name   file name
 * @param   db  dynamic buffer reference
 * @return  0 on success, non-zero otherwise
 */
int writeFile(const char * file_name, dyn_buffer_st * db);

#endif // _FILE_HANDLER_H_