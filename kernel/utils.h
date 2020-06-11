#ifndef MUSHOS_UTILS_H
#define MUSHOS_UTILS_H

#include "generic.h"

void memory_copy (byte* source, byte* dest, u_dword num_bytes);
void memory_clear (byte* start, u_dword num_bytes, byte sample);
void memory_fill (byte* source, byte* dest, u_dword source_bytes, u_dword times);

#endif //MUSHOS_UTILS_H
