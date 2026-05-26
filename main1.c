#include "stream.h"
#include <stdio.h>
#include <assert.h>

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

#define BUF_SIZE 4096

int main() {
    //unsigned char buffer[BUF_SIZE] = {0};
    stream_t *sp = create_stream();
    assert(sp);

    void *ctx = create_memstream_ctx(BUF_SIZE);
    assert(ctx);

    int ret = sp->ops->open(sp, ctx);
    assert(ret >= 0);

    strview_t sv = {0};
    ctsr_to_strview(&sv, "io.h: read, write, lseek, ioctl\n");    
    size_t n = sp->ops->write(sp, sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "fcntl.h: open, close\n");   
    n = sp->ops->write(sp, sv.buf, sv.len);
    assert(n == sv.len);

    long cur = sp->ops->tell(sp);
    sp->ops->lseek(sp, -cur, SEEK_CUR);

    unsigned char buf1[BUF_SIZE] = {0};
    n = sp->ops->read(sp, buf1, cur);
    assert(n == cur);

    printf("%.*s\n", n, buf1);

    sp->ops->close(sp);
    destroy_memstream_ctx(ctx);
    destroy_stream(sp);

    return 0;
}