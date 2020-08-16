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

boolean memory_compare (byte* comp1, byte* comp2, u_dword length) {
    boolean equal = true;
    for (int i = 0; i < length; ++i) {
        equal &= (*(comp1 + i) == *(comp2 + i));
    }
    return equal;
}



u_dword placement_address;

u_dword k_malloc_physical(u_dword sz, boolean align, u_dword *phys) {
    if (align && (placement_address & 0x00000fff)) { // If the address is not already page-aligned, align it.
        placement_address &= 0xfffff000;
        placement_address += 0x1000;
    }
    if (phys) *phys = placement_address;
    u_dword tmp = placement_address;
    placement_address += sz;
    return tmp;
}

u_dword k_malloc(u_dword sz) {
    return k_malloc_physical(sz, false, nullptr);
}

u_dword k_malloc_aligned(u_dword sz, boolean align) {
    return k_malloc_physical(sz, align, nullptr);
}



byte* allocate_memory (u_dword structs_number, u_dword struct_len) {
    return nullptr;
}

void free_memory (byte* pos) {

}
