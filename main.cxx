extern "C" {
#include "stream.h"
#include "strview.h"
#include <stdio.h>
#include <assert.h>
}

#define BUF_SIZE 4096

struct stream_dev {
    stream_t   *stream;
    bool        is_open;
    stream_dev() : stream(nullptr), is_open(false) {};
    stream_t *create_stream() {
        this->stream = ::create_stream();
        if (!this->stream) return nullptr;
        return this->stream;
    }
    int open(void *ctx) {
        if (!this->stream || is_open || !ctx) return -1;
        int ret = this->stream->ops->open(this->stream, ctx);
        if (ret < 0) return -1;
        this->is_open = true;
        return ret;
    }
    size_t read(void *buf, size_t size) {
        if (!this->stream) return 0;
        return this->stream->ops->read(this->stream, buf, size);
    }
    size_t write(const void *buf, size_t size) {
        if (!this->stream) return 0;
        return this->stream->ops->write(this->stream, buf, size);
    }
    long lseek(long off, int origin) {
        if (!this->stream) return 0;
        return this->stream->ops->lseek(this->stream, off, origin);
    }
    long tell() {
        if (!this->stream) return 0;
        return this->stream->ops->tell(this->stream);
    }
    int close() {
        if (!this->stream || !this->is_open) return -1;
        int ret = this->stream->ops->close(this->stream);
        if (ret < 0) return -1;
        this->is_open = false;
        return ret;
    }
    void destroy_stream() {
        if (this->stream) ::destroy_stream(this->stream);
        this->stream = nullptr;
    }
    ~stream_dev() { // guard to prevent leak
        close();
        destroy_stream();
    };
};

struct stream_ctx {
    void   *_ctx;
    stream_ctx() : _ctx(nullptr) {};
    void *create_ctx(size_t size) {
        this->_ctx = ::create_memstream_ctx(size);
        return this->_ctx;
    };
    void *resize_ctx(size_t new_size) {
        void *new_ctx = ::resize_memstream_ctx(this->_ctx, new_size);
        if (!new_ctx) return nullptr;
        this->_ctx = new_ctx;
        return this->_ctx;
    };
    void destroy_ctx() {
        if (this->_ctx) ::destroy_memstream_ctx(this->_ctx);
        this->_ctx = nullptr;
    };
    ~stream_ctx() { // guard to prevent leak
        destroy_ctx();
    };
};


int main() {
    unsigned char buf1[BUF_SIZE] = {0};

    stream_dev stream;
    stream_ctx sc;

    stream_t *sp = stream.create_stream();
    if (!sp) {
        puts("stream.create_stream failed");
        return -1;
    }
    void *ctx = sc.create_ctx(256);
    if (!ctx) {
        puts("sc.create_ctx failed");
        return -1;
    }

    if (stream.open(ctx) < 0) {
        puts("stream.open failed");
        return -1;
    }

    strview_t sv = {0};
    ctsr_to_strview(&sv, "io.h: read, write, lseek, ioctl\n");    
    size_t n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "fcntl.h: open, close\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    long cur = stream.tell();
    stream.lseek(-cur, SEEK_CUR);
    n = stream.read(buf1, cur);
    assert(n == cur);
    printf("%.*s\n", n, buf1);


    stream.close();
    sc.destroy_ctx();

    ctx = sc.create_ctx(BUF_SIZE);
    stream.open(ctx);

    ctsr_to_strview(&sv, "file_ops: open close lseek read write mmap ioctl release\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "mem_ops: alloc free seek getcur setcur\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    cur = stream.tell();
    stream.lseek(-cur, SEEK_CUR);
    n = stream.read(buf1, cur);
    assert(n == cur);
    printf("%.*s\n", n, buf1);

    stream.close();
    sc.destroy_ctx(); // optional
    stream.destroy_stream(); // optional

    return 0;
}