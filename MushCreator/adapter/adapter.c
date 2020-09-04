#include <stdlib.h>
#include <stdio.h>
#include "adapter.h"
#include "../file_system/structures.h"

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



int read_external(char* name, byte** container) {
    FILE* file = fopen(name, "rb");
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    *container = calloc(size, sizeof(byte));
    int read = (int) fread(*container, sizeof(byte), size, file);
    fclose(file);
    return read;
}

int insert_header(char* name) {
    byte* raw_bytes = 0;
    int read = read_external(name, &raw_bytes);
    if (read > fs_header_offset) {
        printf("\nWarning! Kernel size overflow!!\nKernel free space: %d of %d\n", fs_header_offset - read, fs_header_offset);
        return 228;
    } else printf("\nKernel free space: %d of %d\n", fs_header_offset - read, fs_header_offset);

    if (read) {
        FILE* file = fopen(file_name, "r+b");
        int written = (int) fwrite(raw_bytes, read, 1, file);
        fclose(file);
        free(raw_bytes);
        return written;
    } else {
        free(raw_bytes);
        return read;
    }
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
