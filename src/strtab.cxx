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
        cxx_strtab st{};
        st.init();

        st.push("open");
        st.push("close");
        st.push("read");
        st.push("write");
        st.push("lseek");
        st.push("llseek");
        st.push("release");
        st.push("ftruncate");
        st.push("fstat");
        st.push("tell");
        st.push("ioctl");

        strc = st.len();
        for (uint32_t i = 0; i < strc; ++i) {
            std::cout << st[i] << '\n';
        }
        std::cout << "\nstrc: " << strc << "\n\n";

        stream.lseek(0, SEEK_SET);
        stream.write(st.offtab(), strc * sizeof(*st.offtab()));
        stream.write(st.data(), st.data_len());

        st.destroy();
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
