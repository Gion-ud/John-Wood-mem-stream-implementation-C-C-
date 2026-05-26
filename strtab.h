#pragma once

#include <string.h>
#include <stdint.h>

typedef struct _strtab {
    char       *buf_p;
    uint32_t   *off_arr;
    uint32_t    buf_len;
    uint32_t    buf_size;
    uint32_t    strc;
} strtab_t;
