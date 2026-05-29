#pragma once

#include <string.h>
#include <stdint.h>

#define STRTAB_INIT_BUFSIZE 32
#define STRTAB_INIT_TBLSIZE 4

typedef struct _strtab strtab_t;

extern strtab_t *create_strtab(size_t tbl_size);
extern void destroy_strtab(strtab_t *stp);
extern int32_t strtab_push(strtab_t *stp, const char *cstr);
extern const char *strtab_get(strtab_t *stp, uint32_t idx);
extern uint32_t strtab_len(strtab_t *stp);
extern uint32_t strtab_size(strtab_t *stp);
extern uint32_t strtab_data_len(strtab_t *stp);
extern uint32_t strtab_data_size(strtab_t *stp);
extern const uint32_t *strtab_offtab(strtab_t *stp);
extern const char *strtab_data(strtab_t *stp);
extern int strtab_resize(strtab_t *stp, size_t new_size);
extern int is_full_strtab(strtab_t *stp);
#define strtab_grow(stp) (strtab_resize(stp, strtab_size(stp) * 2))


#ifdef __cplusplus
struct cxx_strtab {
    strtab_t *strtab_p;
    inline cxx_strtab() : strtab_p(nullptr) {}
    inline bool init() {
        if (this->strtab_p) return false;
        this->strtab_p = create_strtab(STRTAB_INIT_BUFSIZE);
        return (!!this->strtab_p);
    }
    inline void destroy() {
        if (!this->strtab_p) return;
        destroy_strtab(this->strtab_p);
        this->strtab_p = nullptr;
    }
    inline int32_t push(const char *cstr) {
        if (!cstr) return -1;
        if (is_full_strtab(this->strtab_p)) {
            if (strtab_grow(this->strtab_p) < 0) return -1;
        }
        return strtab_push(this->strtab_p, cstr);
    }
    inline const char *get(uint32_t idx) {
        return strtab_get(this->strtab_p, idx);
    }
    inline const char *operator[](uint32_t idx) {
        return strtab_get(this->strtab_p, idx);
    }
    inline uint32_t len() {
        return strtab_len(this->strtab_p);
    }
    inline const uint32_t *offtab() {
        return strtab_offtab(this->strtab_p);
    }
    inline const char *data() {
        return strtab_data(this->strtab_p);
    }
    inline uint32_t data_len() {
        return strtab_data_len(this->strtab_p);
    }
    inline ~cxx_strtab() {
        destroy_strtab(this->strtab_p);
    }
};
#endif /* __cplusplus */

#define __STRTAB_IMPLM__
#ifdef __STRTAB_IMPLM__

struct _strtab {
    char       *buf;
    uint32_t   *off_arr;
    uint32_t    buf_len;
    uint32_t    buf_size;
    uint32_t    tbl_len;
    uint32_t    tbl_size;
};

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

int is_full_strtab(strtab_t *stp) {
    return (stp)->tbl_len >= (stp)->tbl_size;
}

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

int strtab_resize(strtab_t *stp, size_t new_size) {
    if (!is_valid_strtab(stp) || !new_size) return -1;
    uint32_t *new_offtab = (uint32_t*)realloc(stp->off_arr, new_size * sizeof(uint32_t));
    if (!new_offtab) return -1;
    stp->off_arr = new_offtab;
    stp->tbl_size = new_size;
    if (stp->tbl_len > new_size) stp->tbl_len = new_size;
    return 0;
}

const char *strtab_get(strtab_t *stp, uint32_t idx) {
    if (!is_valid_strtab(stp) || idx >= stp->tbl_len) return NULL;
    return stp->buf + stp->off_arr[idx];
}
const uint32_t *strtab_offtab(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? NULL : stp->off_arr;
}
const char *strtab_data(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? NULL : stp->buf;
}
uint32_t strtab_len(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? 0 : stp->tbl_len;
}
uint32_t strtab_size(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? 0 : stp->tbl_size;
}
uint32_t strtab_data_len(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? 0 : stp->buf_len;
}
uint32_t strtab_data_size(strtab_t *stp) {
    return (!is_valid_strtab(stp)) ? 0 : stp->buf_size;
}

#include <stdio.h>
void destroy_strtab(strtab_t *stp) {
    if (!stp) return;
    if (stp->buf) free(stp->buf);
    if (stp->off_arr) free(stp->off_arr);
    free(stp);
}

#endif /*__STRTAB_IMPLM__*/

