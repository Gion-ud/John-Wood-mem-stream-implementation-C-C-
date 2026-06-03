all: stream.o strtab

CFLAGS = -O2 -Wall -Wextra -Werror -fno-exceptions -fno-strict-aliasing -Wno-unused-variable
CXXFLAGS = -fno-rtti


CC = gcc

stream.o: src/stream.c | build
	$(CC) -c $< -o build/$@ $(CFLAGS)

main: src/main.cxx build/stream.o | bin
	$(CC) $^ -o bin/$@ $(CFLAGS) -Llib -lgcc_s -lstdc++

strtab: src/strtab.cxx build/stream.o | bin
	$(CC) $^ -o bin/$@ $(CFLAGS) -Llib -lgcc_s -lstdc++
