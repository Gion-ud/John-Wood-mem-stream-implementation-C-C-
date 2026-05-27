extern "C" {
#include <stdio.h>
#include <assert.h>
}
#include "stream.hpp"
#define BUF_SIZE 4096


int main() {
    unsigned char buf1[BUF_SIZE] = {0};

    stream_dev stream;
    stream_ctx sc;

    stream_t *sp = stream.create_stream();
    if (!sp) {
        puts("cannot create stream");
        return -1;
    }
    void *ctx = sc.create_ctx(BUF_SIZE);
    if (!ctx) {
        puts("cannot create stream ctx");
        return -1;
    }
    if (stream.open(ctx) < 0) {
        puts("cannot open stream ctx");
        return -1;
    }

    strview_t sv{};
    ctsr_to_strview(&sv, "io.h: read, write, lseek, ioctl\n"); 
    size_t n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "fcntl.h: open, close\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    long cur = stream.tell();
    stream.lseek(-cur, SEEK_CUR);
    n = stream.read(buf1, cur);
    assert((long)n == cur);
    printf("%.*s\n", n, buf1);
    long brkpoint = cur;

    ctsr_to_strview(&sv, "file_ops: open close lseek read write mmap ioctl release\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    ctsr_to_strview(&sv, "mem_ops: alloc free seek getcur setcur\n");   
    n = stream.write(sv.buf, sv.len);
    assert(n == sv.len);

    cur = stream.tell();
    stream.lseek(-(cur - brkpoint), SEEK_END);
    n = stream.read(buf1, cur);
    assert((long)n == cur - brkpoint);
    printf("%.*s\n", n, buf1);

    stream.close();
    sc.destroy_ctx();
    stream.destroy_stream();

    return 0;
}
