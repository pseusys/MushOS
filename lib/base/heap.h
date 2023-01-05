#ifndef MUSHLIB_HEAP_H
#define MUSHLIB_HEAP_H

#include "generic.h"


typedef struct {
    u_dword size;
    void* previous, * next;
} heap_block_header;

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

#endif // MUSHLIB_HEAP_H
