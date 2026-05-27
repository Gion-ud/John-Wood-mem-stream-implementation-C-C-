#pragma once

extern "C" {
#include "stream.h"
#include "strview.h"
}


struct stream_dev {
    stream_t   *stream;
    bool        is_open;

    inline stream_dev() : stream(nullptr), is_open(false) {};
    inline stream_t *create_stream() {
        if (this->stream) return this->stream;
        this->stream = ::create_stream();
        if (!this->stream) return nullptr;
        return this->stream;
    }
    inline int open(void *ctx) {
        if (!this->stream || is_open || !ctx) return -1;
        int ret = this->stream->ops->open(this->stream, ctx);
        if (ret < 0) return -1;
        this->is_open = true;
        return ret;
    }
    inline size_t read(void *buf, size_t size) {
        if (!this->stream || !this->is_open) return 0;
        return this->stream->ops->read(this->stream, buf, size);
    }
    inline size_t write(const void *buf, size_t size) {
        if (!this->stream || !this->is_open) return 0;
        return this->stream->ops->write(this->stream, buf, size);
    }
    inline long lseek(long off, int origin) {
        if (!this->stream || !this->is_open) return 0;
        return this->stream->ops->lseek(this->stream, off, origin);
    }
    inline long tell() {
        if (!this->stream || !this->is_open) return -1;
        return this->stream->ops->tell(this->stream);
    }
    inline int close() {
        if (!this->stream || !this->is_open) return -1;
        int ret = this->stream->ops->close(this->stream);
        if (ret < 0) return -1;
        this->is_open = false;
        return ret;
    }
    inline void destroy_stream() {
        if (this->stream) ::destroy_stream(this->stream);
        this->stream = nullptr;
    }
    inline ~stream_dev() { // guard to prevent leak
        close();
        destroy_stream();
    };
};

struct stream_ctx {
    void   *_ctx;

    inline stream_ctx() : _ctx(nullptr) {};
    inline void *create_ctx(size_t size) {
        if (this->_ctx) return this->_ctx;
        this->_ctx = ::create_memstream_ctx(size);
        return this->_ctx;
    };
    inline void *resize_ctx(size_t new_size) {
        void *new_ctx = ::resize_memstream_ctx(this->_ctx, new_size);
        if (!new_ctx) return nullptr;
        this->_ctx = new_ctx;
        return this->_ctx;
    };
    inline void destroy_ctx() {
        if (this->_ctx) ::destroy_memstream_ctx(this->_ctx);
        this->_ctx = nullptr;
    };
    inline ~stream_ctx() { // guard to prevent leak
        destroy_ctx();
    };
};
