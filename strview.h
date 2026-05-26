#pragma once

#include <stddef.h>
#include <string.h>

typedef struct  _strview {
    const void *buf;
    size_t      len;
} strview_t;

static inline strview_t *ctsr_to_strview(
    strview_t  *strview_p,
    const char *cstr
) {
    return
        (!strview_p || !cstr) ? NULL : ({
            strview_p->buf = cstr;
            strview_p->len = strlen(cstr);
            strview_p;
        });
}
