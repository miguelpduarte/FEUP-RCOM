#ifndef _FILE_HANDLER_H_
#define _FILE_HANDLER_H_

#include "dyn_buffer.h"

int readFile(const char * file_name, dyn_buffer_st* db);

int writeFile(const char * file_name, dyn_buffer_st * db);

#endif // _FILE_HANDLER_H_