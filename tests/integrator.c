#include "integrator.h"

#include <mushlib.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


void* allocate_space_for_heap(int size) {
    return calloc(1, size);
}

void check_heap_for_leaks() {
    precise hep_leak = occupation();
    printf("Occupation: %f%%.\n", hep_leak);
    assert(hep_leak == 0);
}
