#include <stdlib.h>
#include <stdio.h>
#include "adapter.h"

const char* file_name = "./fs/FS.binary";
const int fs_size = 4194304;

void create_fs() {
    FILE* file = fopen(file_name, "a+b");
    byte* zero = calloc(1, sizeof(byte));
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    printf("Size to be added: %ld\n", fs_size - size);
    if (size < fs_size) for (long i = 0; i < fs_size - size; ++i) fwrite(zero, sizeof(byte), 1, file);
    printf("FS file_header ready!\n");
    fclose(file);
}



int read_out_file(char* name, byte* raw_bytes) {
    FILE* file = fopen(name, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);

    raw_bytes = calloc(size, sizeof(byte));
    int written = (int) fread(raw_bytes, sizeof(byte), size, file);

    fclose(file);
    return written;
}

int write_out_file(char* name, byte* raw_bytes, int size) {
    FILE* file = fopen(name, "a+b");
    int written = (int) fwrite(raw_bytes, sizeof(byte), size, file);
    fclose(file);
    return written;
}



void write_at(int pos, byte info) {
    FILE* file = fopen(file_name, "r+b");

    fseek(file, pos, SEEK_SET);
    byte* raw_byte = calloc(1, sizeof(byte));
    raw_byte[0] = info;
    fwrite(raw_byte, sizeof(byte), 1, file);

    fclose(file);
}

byte read_at(int pos) {
    FILE* file = fopen(file_name, "rb");

    fseek(file, pos, SEEK_SET);
    byte* raw_byte = calloc(1, sizeof(byte));
    fread(raw_byte, sizeof(byte), 1, file);

    fclose(file);
    return *raw_byte;
}



long get_drive_size() {
    return fs_size;
}
