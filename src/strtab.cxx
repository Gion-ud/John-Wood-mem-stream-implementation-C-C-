extern "C" {
#define __STRTAB_IMPLM__
#include "strtab.h"
}
#include <stdio.h>
#include <assert.h>
#include "stream.hpp"
#include <iostream>

#define BUF_SIZE 4096


int main() {
    stream_dev stream{};
    stream_ctx sc{};

    if (!stream.create_stream()) {
        puts("cannot create stream");
        return -1;
    }
    if (!sc.create_ctx(BUF_SIZE)) {
        puts("cannot create stream ctx");
        return -1;
    }
    if (stream.open(&sc) < 0) {
        puts("cannot open stream ctx");
        return -1;
    }

    uint32_t strc{};
    {
        strtab_t *stp = create_strtab(16);
        assert(stp);

        strtab_push(stp, "open");
        strtab_push(stp, "close");
        strtab_push(stp, "read");
        strtab_push(stp, "write");
        strtab_push(stp, "lseek");
        strtab_push(stp, "mmap");
        strtab_push(stp, "munmap");
        strtab_push(stp, "release");
        strtab_push(stp, "truncate");

        strc = strtab_size(stp);
        for (uint32_t i = 0; i < strc; ++i) {
            std::cout << strtab_get(stp, i) << '\n';
        }
        std::cout << '\n';

        stream.lseek(0, SEEK_SET);
        stream.write(stp->off_arr, stp->tbl_len * sizeof(*stp->off_arr));
        stream.write(stp->buf, stp->buf_len);

        destroy_strtab(stp);
    }

    stream_map_t sm{};
    stream_map_init(&sm, BUF_SIZE, 0);
    stream.mmap(&sm);

    char *strtab_begin = static_cast<char*>(sm.addr) + strc * sizeof(uint32_t);

    for (uint32_t i = 0; i < strc; ++i) {
        std::cout << strtab_begin + reinterpret_cast<uint32_t*>(sm.addr)[i] << '\n';
    }


    stream.close(&sc);
    sc.destroy_ctx();
    stream.destroy_stream();

    return 0;
}
