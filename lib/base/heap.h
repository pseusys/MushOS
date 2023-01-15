#ifndef MUSHLIB_HEAP_H
#define MUSHLIB_HEAP_H

#include "generic.h"


#define allocation_exception_id 0x1
#define allocation_exception_type "Allocation Exception"

#define heap_exception_id 0x2
#define heap_exception_type "Heap Exception"


#define size_of(structure) (size(structure) / sizeof(typeof(*structure)))

precise occupation();

void initialize_heap(void* start_address, u_dword size);

void* ralloc(u_dword size);

void* zalloc(u_dword size);

u_dword size(void* structure);

void* challoc(void* structure, u_dword new_size);

void unalloc(void* structure);

#endif // MUSHLIB_HEAP_H
