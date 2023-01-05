#ifndef MUSHOS_PLACEMENT_H
#define MUSHOS_PLACEMENT_H

#include "../../lib/base/generic.h"


void* k_malloc(u_dword sz);
void* k_malloc_aligned(u_dword sz, boolean align);
void* k_malloc_physical(u_dword sz, boolean align, u_dword *phys);

#endif //MUSHOS_PLACEMENT_H
