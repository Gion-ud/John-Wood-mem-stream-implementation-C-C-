all: stream.o main

CFLAGS = -O2 -Wall -Wextra -Werror -fno-exceptions -fno-strict-aliasing
CXXFLAGS = -fno-rtti

stream.o: src/stream.c | build
	cc -c $< -o build/$@ $(CFLAGS)

main: src/main.cxx build/stream.o | bin
	cc $^ -o bin/$@ $(CFLAGS) -Llib -lgcc -lgcc_s -lstdc++

