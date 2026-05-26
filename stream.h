#pragma once

#include <stddef.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

typedef struct _stream_ops {
    int     (*open)(void *stream, void *ctx);
    int     (*close)(void *stream);
    size_t  (*read)(void *stream, void *buf, size_t size);
    size_t  (*write)(void *stream, const void *buf, size_t size);
    long    (*lseek)(void *stream, long off, int origin);
    long    (*tell)(void *stream);
} stream_ops_t;

typedef struct _stream_ctx {
    void   *buf;
    long    pos;
    long    len;
    long    size;
} stream_ctx_t;

typedef struct _file_ctx {
	int 	fd;
	long    pos;
    long    len;
} file_ctx_t;

typedef struct _stream {
    void           *ctx; // stream_ctx
    stream_ops_t   *ops;
} stream_t;


extern void *stream_ctx_bind(stream_ctx_t *ctx, void *buf, size_t size);
extern void stream_ctx_reset(stream_ctx_t *ctx);
extern stream_t *stream_init(stream_t *sp);
extern void stream_fini(stream_t *sp);

#include <string.h>

#if __STDC_HOSTED__ == 1
#include <stdlib.h>
extern stream_t *create_stream();
extern void destroy_stream(stream_t *stream_p);

extern void *create_memstream_ctx(size_t size);
extern void *resize_memstream_ctx(void *ctx, size_t new_size);
extern void destroy_memstream_ctx(void *ctx);

#endif
