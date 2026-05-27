#include "stream.h"
#include <stdio.h>
#include <assert.h>
#include "strview.h"

#define BUF_SIZE 4096

int main() {
    int main_ret = 0;
    unsigned char buf1[BUF_SIZE] = {0};

    stream_t *sp = create_stream();
    if (!sp) {
        puts("create_stream failed");
        main_ret = -1;
        goto main_ret;
    }
    void *ctx = create_memstream_ctx(BUF_SIZE);
    if (!ctx) {
        puts("create_memstream_ctx failed");
        main_ret = -1;
        goto destroy_stream;
    }
    int ret = sp->ops->open(sp, ctx);
    if (ret < 0) {
        puts("sp->ops->open failed");
        main_ret = -1;
        goto destroy_ctx;
    }

    strview_t sv = {0};
    ctsr_to_strview(&sv, "io.h: read, write, lseek, ioctl\n");    
    size_t n = sp->ops->write(sp, sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "fcntl.h: open, close\n");   
    n = sp->ops->write(sp, sv.buf, sv.len);
    assert(n == sv.len);

    long cur = sp->ops->tell(sp);
    sp->ops->lseek(sp, -cur, SEEK_CUR);
    n = sp->ops->read(sp, buf1, cur);
    assert(n == cur);
    printf("%.*s\n", n, buf1);


close_stream:
    sp->ops->close(sp);
destroy_ctx:
    destroy_memstream_ctx(ctx);
destroy_stream:
    destroy_stream(sp);
main_ret:

    return main_ret;
}