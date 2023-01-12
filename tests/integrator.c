#include "integrator.h"

#include <stdlib.h>


void* allocate_space_for_heap(int size) {
    return calloc(1, size);
}
