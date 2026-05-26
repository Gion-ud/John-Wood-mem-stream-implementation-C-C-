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
    unsigned char buffer[BUF_SIZE] = {0};
    stream_t s = {0};
    stream_ctx_t ctx = {0};
    stream_t *sp = stream_init(&s);
    stream_ctx_bind(&ctx, buffer, sizeof(buffer));

    int ret = sp->ops->open(sp, &ctx);
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

    ret = sp->ops->close(sp);
    assert(ret >= 0);
    stream_ctx_reset(&ctx);
    stream_fini(sp);

    return 0;
}