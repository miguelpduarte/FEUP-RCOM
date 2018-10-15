#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

typedef unsigned char byte;
#define MIN(x,y)    (((x) < (y)) ? (x) : (y))
#define MAX(x,y)    (((x) > (y)) ? (x) : (y))

byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index);

#endif // _UTILS_H_