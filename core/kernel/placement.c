#include "placement.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/stdio.h"


u_dword placement_address = 0x12000;

void* k_malloc_physical(u_dword sz, boolean align, u_dword *phys) {
    if (align && (placement_address & 0x00000fff)) { // If the address is not already page-aligned, align it.
        placement_address &= 0xfffff000;
        placement_address += 0x1000;
    }
    if (phys) *phys = placement_address;
    u_dword tmp = placement_address;
    placement_address += sz;
    return (void*) tmp;
}

void* k_malloc(u_dword sz) {
    return k_malloc_physical(sz, false, nullptr);
}

void* k_malloc_aligned(u_dword sz, boolean align) {
    return k_malloc_physical(sz, align, nullptr);
}

void get_placement_address() {
    info("Placement address: %h\n", placement_address)
}
