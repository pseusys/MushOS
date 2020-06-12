#ifndef MUSHOS_MEMORY_H
#define MUSHOS_MEMORY_H

#include "../kernel/generic.h"

void memory_copy (byte* source, byte* dest, u_dword num_bytes);
void memory_clear (byte* start, u_dword num_bytes, byte sample);
void memory_fill (byte* source, byte* dest, u_dword source_bytes, u_dword times);

byte* allocate_memory (u_dword structs_number, u_dword struct_len);
void free_memory (byte* pos);

#endif //MUSHOS_MEMORY_H
