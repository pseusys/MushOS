#ifndef MUSHOS_HEAP_H
#define MUSHOS_HEAP_H

#include "generic.h"

typedef struct heap_block_header heap_block_header;

struct heap_block_header {
    u_dword size;
    void* previous, * next;
};

typedef struct {
    void* first_address;
    void* heap_start, * heap_end;
} heap_header;

#define size_of(structure) (size(structure) / sizeof(typeof(*structure)))



void initialize_heap(void* start_address, u_dword size);

void* malloc(u_dword size);

u_dword size(void* structure);

void* realloc(void* structure, u_dword new_size);

void free(void* structure);
void test_free(void* structure);

void print_heap();

#endif //MUSHOS_HEAP_H
