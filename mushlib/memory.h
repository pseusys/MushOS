#ifndef MUSHOS_MEMORY_H
#define MUSHOS_MEMORY_H

#include "../kernel/generic.h"

void memory_copy (byte* source, byte* dest, u_dword num_bytes);
void memory_clear (byte* start, u_dword num_bytes, byte sample);
void memory_fill (byte* source, byte* dest, u_dword source_bytes, u_dword times);
boolean memory_compare (byte* comp1, byte* comp2, u_dword length);

void* k_malloc(u_dword sz);
void* k_malloc_aligned(u_dword sz, boolean align);
void* k_malloc_physical(u_dword sz, boolean align, u_dword *phys);

#endif //MUSHOS_MEMORY_H
