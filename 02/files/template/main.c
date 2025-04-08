#include <stdio.h>
#include <stdlib.h>

#include "main.h"

// iconv -f cp1251 -t utf-8 files/1/cp1251.txt > files/1/cp1251_utf8.txt
// iconv -f koi8 -t utf-8 files/1/koi8.txt > files/1/koi8_utf8.txt
const char *input_file_path = NULL;
const char *encoding = NULL;
const char *output_file_path = NULL;

int main(int argc, char** argv){
    if (argc < 2 || argc != 4) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    input_file_path = argv[1];
    encoding = argv[2];
    output_file_path = argv[3];

    printf("input_file_path = %s\n", input_file_path);
    printf("encoding = %s\n", encoding);
    printf("output_file_path = %s\n", output_file_path);

    int encode_index = check_encoding(encoding);
    //printf("encode_index = %d\n", encode_index);
    if (encode_index < 0) {
        printf("Неизвестная кодировка: %s\n", encoding);
        return EXIT_FAILURE;
    }

    FILE* input_file = fopen(input_file_path, "rb");
    if(!input_file)
    {
        printf("Не удалось открыть %s для чтения!\n", input_file_path);
        return EXIT_FAILURE;
    }

    FILE* output_file = fopen(output_file_path, "wb");
    if(!output_file)
    {
        printf("Не удалось открыть %s для записи!\n", output_file_path);
        return EXIT_FAILURE;
    }

    int input_ch;
    const unsigned int *en_t = encode_table[encode_index];

    while ((input_ch = fgetc(input_file)) != EOF) {
        if (input_ch < 0x80) {
            fputc(input_ch, output_file);
        } else {
            unsigned int output_ch = en_t[input_ch - 0x80];
            if (output_ch < 0x800) {
                fputc(0xC0 | (output_ch >> 6), output_file);
                fputc(0x80 | (output_ch & 0x3F), output_file);
            } else if (output_ch < 0x10000) {
                fputc(0xE0 | (output_ch >> 12), output_file);
                fputc(0x80 | ((output_ch >> 6) & 0x3F), output_file);
                fputc(0x80 | (output_ch & 0x3F), output_file);
            } else {
                fputc(0xF0 | (output_ch >> 18), output_file);
                fputc(0x80 | ((output_ch >> 12) & 0x3F), output_file);
                fputc(0x80 | ((output_ch >> 6) & 0x3F), output_file);
                fputc(0x80 | (output_ch & 0x3F), output_file);
            }
        }
    }

    fclose(input_file);
    fclose(output_file);

    return EXIT_SUCCESS;
}
