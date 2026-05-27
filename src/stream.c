#include "stream.h"

extern int stream_open(void *stream, void *ctx);
extern int stream_close(void *stream);
extern size_t stream_read(void *stream, void *buf, size_t size);
extern size_t stream_write(void *stream, const void *buf, size_t size);
extern long stream_lseek(void *stream, long off, int origin);
extern long stream_tell(void *stream);

static void nop_fn(void) {};

//int (*flush)(void *stream);
//int (*ioctl)(void *stream, unsigned int cmd, void *arg);

static stream_ops_t stream_ops = {
    .open   = &stream_open,
    .close  = &stream_close,
    .read   = &stream_read,
    .write  = &stream_write,
    .lseek  = &stream_lseek,
    .tell   = &stream_tell,
    .flush  = (int (*)(void *))&nop_fn,
    .ioctl  = (int (*)(void *, unsigned int, void *))&nop_fn, 
};

void *stream_ctx_bind(stream_ctx_t *ctx, void *buf, size_t size) {
    if (!ctx || !buf) return NULL;
    ctx->buf    = buf;
    ctx->pos    = 0;
    ctx->len    = 0;
    ctx->size   = size;
    return ctx;
}
void stream_ctx_reset(stream_ctx_t *ctx) {
    if (!ctx) return;
    ctx->pos    = 0;
    ctx->len    = 0;
}

stream_t *stream_init(stream_t *sp) {
    if (!sp) return NULL;
    sp->ops = &stream_ops;
    return sp;
}
void stream_fini(stream_t *sp) {
    if (!sp) return;
    sp->ctx = NULL;
    sp->ops = NULL;
}

int stream_open(void *stream, void *ctx) {
    if (!stream || !ctx) return -1;
    stream_t *_stream_p = (stream_t*)stream;
    _stream_p->ctx = ctx;
    return 0;
}
int stream_close(void *stream) {
    if (!stream) return -1;
    stream_t *_stream_p = (stream_t*)stream;
    _stream_p->ctx = NULL;
    return 0;
}
size_t stream_read(void *stream, void *buf, size_t size) {
    if (!stream || !buf || !size) return 0;
    stream_ctx_t *ctx = (stream_ctx_t*)((stream_t*)stream)->ctx;
    if (!ctx) return 0;
    long rem = ctx->len - ctx->pos;
    if (rem <= 0) return 0;
    long n = ((long)size <= rem) ? (long)size : rem;
    memcpy(
        buf,
        (unsigned char*)ctx->buf + ctx->pos,
        n
    );
    ctx->pos += n;
    return n;
}
size_t stream_write(void *stream, const void *buf, size_t size) {
    if (!stream || !buf || !size) return 0;
    stream_ctx_t *ctx = (stream_ctx_t*)((stream_t*)stream)->ctx;
    if (!ctx) return 0;
    long rem = ctx->size - ctx->pos;
    if (rem < 0) return 0;
    long n = ((long)size <= rem) ? (long)size : rem;
    memcpy(
        (unsigned char*)ctx->buf + ctx->pos,
        buf,
        n
    );
    ctx->pos += n;
    if (ctx->len < ctx->pos)
        ctx->len = ctx->pos;
    return n;
}

long stream_lseek(void *stream, long off, int origin) {
    if (!stream) return -1;
    stream_ctx_t *ctx = (stream_ctx_t*)((stream_t*)stream)->ctx;
    if (!ctx) return -1;
    long _off = 0;
    switch (origin) {
        case (SEEK_SET):
            _off = off;
            break;
        case (SEEK_CUR):
            _off = (long)ctx->pos + off;
            break;
        case (SEEK_END):
            _off = (long)ctx->len + off;
            break;
        default:
            return -1;
    }
    ctx->pos = _off;
    return ctx->pos;
}
long stream_tell(void *stream) {
    if (!stream) return -1;
    stream_ctx_t *ctx = (stream_ctx_t*)((stream_t*)stream)->ctx;
    return ctx->pos;
}

#if __STDC_HOSTED__ == 1
#include <assert.h>

stream_t *create_stream() {
    stream_t *sp = (stream_t*)malloc(sizeof(stream_t));
    if (!sp) return NULL;
    sp->ctx = calloc(1, sizeof(stream_ctx_t));
    if (!sp->ctx) {
        free(sp);
        return NULL;
    }
    sp->ops = &stream_ops;
    return sp;
}
void destroy_stream(stream_t *sp) {
    if (!sp) return;
    if (sp->ctx) free(sp->ctx);
    free(sp);
}
void *create_memstream_ctx(size_t size) {
    stream_ctx_t *ctx = (stream_ctx_t*)malloc(size);
    if (!ctx) return NULL;
    ctx->buf = malloc(size);
    if (!ctx->buf) {
        free(ctx);
        return NULL;
    }
    ctx->pos    = 0;
    ctx->len    = 0;
    ctx->size   = size;

    return ctx;
}
void *resize_memstream_ctx(void *ctx, size_t new_size) {
    if (!ctx || !new_size) return NULL;
    stream_ctx_t *_ctx = (stream_ctx_t*)ctx;
    void *new_buf = realloc(_ctx->buf, new_size);
    if (!new_buf) return NULL;
    _ctx->buf = new_buf;
    _ctx->size = new_size;
    return ctx;
}
void destroy_memstream_ctx(void *ctx) {
    if (!ctx) return;
    stream_ctx_t *_ctx = (stream_ctx_t*)ctx;
    if (_ctx->buf) free(_ctx->buf);
    free(ctx);
}

#endif
