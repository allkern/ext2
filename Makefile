.ONESHELL:

CC := gcc
CFLAGS := -Ofast -ffast-math -Wno-unused-result
SOURCES := main.c
SOURCES += $(wildcard src/*.c)

bin/ext2 main.c:
	mkdir -p bin

	$(CC) $(SOURCES) \
		-o bin/ext2 $(CFLAGS) -Isrc

clean:
	rm -rf "bin"