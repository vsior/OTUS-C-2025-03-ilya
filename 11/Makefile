BUILD_DIR = build

all: clean gtk-tree

gtk-tree: main.c
	$(CC) $(shell pkg-config --cflags gtk4) $^ -o ${BUILD_DIR}/$@ -Wall -Wextra -Wpedantic -std=c11 $(shell pkg-config --libs gtk4)

clean:
	rm -f ${BUILD_DIR}/*

.PHONY: all clean