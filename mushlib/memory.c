#include "memory.h"

void memory_copy (byte* source, byte* dest, u_dword num_bytes) {
    for (int i = 0; i < num_bytes; ++i) {
        *(dest + i) = *(source + i);
    }
}

void memory_clear (byte* start, u_dword num_bytes, byte sample) {
    for (int i = 0; i < num_bytes; ++i) {
        *(start + i) = sample;
    }
}

void memory_fill (byte* dest, byte* source, u_dword source_bytes, u_dword times) {
    for (int i = 0; i < times; ++i) {
        for (int j = 0; j < source_bytes; ++j) {
            *(dest + (i * source_bytes + j)) = *(source + j);
        }
    }
}

byte* allocate_memory (u_dword structs_number, u_dword struct_len) {
    return nullptr;
}

void free_memory (byte* pos) {

}
