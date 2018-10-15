#include <stdlib.h>
#include "utils.h"

byte calcBcc2(byte * data, const size_t data_size, const size_t data_start_index) {
    byte bcc2 = data[data_start_index];
    size_t i = data_start_index + 1;

    for(; i < data_start_index + data_size; ++i) {
        bcc2 ^= data[i];
    }

    return bcc2;
}