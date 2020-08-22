#ifndef MUSHOS_HEAP_H
#define MUSHOS_HEAP_H

#include "generic.h"

typedef struct block_header block_header;

struct block_header {
    u_dword size;
    void* previous, * next;
};

typedef struct {
    block_header* first_address;
    void* heap_start, * heap_end;
} heap_header;

#define size_of(structure) (size(structure) / sizeof(typeof(*structure)))



void initialize_heap(void* start_address, u_dword size);

void* malloc(u_dword size);

u_dword size(void* structure);

void* realloc(void* structure, u_dword new_size);

void free(void* structure);

#endif //MUSHOS_HEAP_H
