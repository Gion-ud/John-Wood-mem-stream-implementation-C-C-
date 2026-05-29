all: stream.o strtab

CFLAGS = -O2 -Wall -Wextra -Werror -fno-exceptions -fno-strict-aliasing -Wno-unused-variable
CXXFLAGS = -fno-rtti

stream.o: src/stream.c | build
	cc -c $< -o build/$@ $(CFLAGS)

main: src/main.cxx build/stream.o | bin
	cc $^ -o bin/$@ $(CFLAGS) -Llib -lgcc -lgcc_s -lstdc++

strtab: src/strtab.cxx build/stream.o | bin
	cc $^ -o bin/$@ $(CFLAGS) -Llib -lgcc -lgcc_s -lstdc++