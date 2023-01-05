#ifndef MUSHLIB_MEMORY_H
#define MUSHLIB_MEMORY_H

#include "generic.h"


void memory_copy (byte* source, byte* dest, u_dword num_bytes);
void memory_clear (byte* start, u_dword num_bytes, byte sample);
void memory_fill (byte* source, byte* dest, u_dword source_bytes, u_dword times);
boolean memory_compare (byte* comp1, byte* comp2, u_dword length);

#endif // MUSHLIB_MEMORY_H
