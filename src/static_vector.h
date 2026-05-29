#pragma once

#include <stddef.h>

#ifdef __STDC__

typedef struct __vector_header {
    size_t  elem_size;
    size_t  vec_len;
    size_t  vec_size;
} _vector_header_t;

static inline void _vector_init_header(
    _vector_header_t   *vec_hdr_p,
    size_t              elem_size,
    size_t              vec_size
) {
    if (!vec_hdr_p || !elem_size || !vec_size)
        return;
    vec_hdr_p->elem_size    = elem_size;
    vec_hdr_p->vec_len      = 0;
    vec_hdr_p->vec_size     = vec_size;
}

static inline void _vector_bind(
    _vector_header_t   *vec_hdr_p,
    void               *buf
) {
    if (!vec_hdr_p || !buf)
        return;
    
}



#endif /*__STDC__*/