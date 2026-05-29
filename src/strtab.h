#pragma once

#include <string.h>
#include <stdint.h>

typedef struct _strtab strtab_t;

extern strtab_t *create_strtab(size_t tbl_size);
extern void destroy_strtab(strtab_t *stp);
extern int32_t strtab_push(strtab_t *stp, const char *cstr);
extern const char *strtab_get(strtab_t *stp, uint32_t idx);

#define __STRTAB_IMPLM__
#ifdef __STRTAB_IMPLM__

#define STRTAB_INIT_BUFSIZE 1
#define STRTAB_INIT_TBLSIZE 1

struct _strtab {
    char       *buf;
    uint32_t   *off_arr;
    uint32_t    buf_len;
    uint32_t    buf_size;
    uint32_t    tbl_len;
    uint32_t    tbl_size;
};

#define strtab_size(stp) ((stp)->tbl_len)
#define strtab_data(stp) ((stp)->buf)
#define strtab_data_len(stp) ((stp)->buf_len)

#include <stdlib.h>
#include <string.h>
strtab_t *create_strtab(size_t tbl_size) {
    uint32_t buf_size = STRTAB_INIT_BUFSIZE;
    if (tbl_size < STRTAB_INIT_TBLSIZE)
        tbl_size = STRTAB_INIT_TBLSIZE;
    strtab_t *stp = (strtab_t*)malloc(sizeof(strtab_t));
    if (!stp) goto fail;
    stp->buf = (char*)malloc(buf_size);
    stp->off_arr = (uint32_t*)calloc(tbl_size, sizeof(uint32_t));
    if (!stp->buf || !stp->off_arr) goto fail;
    stp->buf_len    = 0;
    stp->buf_size   = buf_size;
    stp->tbl_len    = 0;
    stp->tbl_size   = tbl_size;
    return stp;
fail:
    destroy_strtab(stp);
    return NULL;
}

#define is_valid_strtab(stp) \
    (\
        (stp) && (stp)->buf &&\
        (stp)->off_arr && (stp)->buf_size &&\
        (stp)->tbl_size\
    )

#define rem_buf_size_strtab(stp) \
    (\
        (int32_t)(stp)->buf_size - (int32_t)(stp)->buf_len\
    )

#define is_full_strtab(stp) \
    ((stp)->tbl_len >= (stp)->tbl_size)

int32_t strtab_push(strtab_t *stp, const char *cstr) {
    if (!is_valid_strtab(stp) || !cstr || is_full_strtab(stp))
        return -1;
    uint32_t cstr_size = strlen(cstr) + 1;
    if ((int32_t)cstr_size > rem_buf_size_strtab(stp)) {
        uint32_t new_buf_size =
            (stp->buf_size >= cstr_size)
            ? stp->buf_size * 2 : cstr_size * 2;
        char *new_buf = (char*)realloc(stp->buf, new_buf_size);
        if (!new_buf) return -1;
        stp->buf_size = new_buf_size;
        stp->buf = new_buf;
    }
    uint32_t _cstr_off = stp->buf_len;
    char *_cstr = stp->buf + stp->buf_len;
    memcpy(
        _cstr,
        cstr,
        cstr_size - 1
    );
    _cstr[cstr_size - 1] = '\0';
    stp->buf_len += cstr_size;

    int32_t cstr_idx = stp->tbl_len;
    stp->off_arr[cstr_idx] = _cstr_off;
    ++stp->tbl_len;

    return cstr_idx;
}

const char *strtab_get(strtab_t *stp, uint32_t idx) {
    if (!is_valid_strtab(stp) || idx >= stp->tbl_len) goto fail;
    return stp->buf + stp->off_arr[idx];
fail:
    return NULL;
}

#include <stdio.h>
void destroy_strtab(strtab_t *stp) {
    if (!stp) return;
    if (stp->buf) free(stp->buf);
    if (stp->off_arr) free(stp->off_arr);
    free(stp);
}

#endif /*__STRTAB_IMPLM__*/

