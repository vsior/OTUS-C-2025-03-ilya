BUILD_DIR = build
all: clean weather

weather: main.c
	$(CC) $^ -o ${BUILD_DIR}/$@ -Wall -Wextra -Wpedantic -lcurl -lcjson -std=c11

clean:
	rm -f ${BUILD_DIR}/*

.PHONY: all clean