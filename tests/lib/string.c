#include <mushlib.h>

#include <assert.h>
#include <stdio.h>

#include "../integrator.h"


int main() {
    void* heap = allocate_space_for_heap(heap_size);
    initialize_heap(heap, heap_size);
    mod_string str;

    str = concatenate("String1", "String2");
    printf("Testing concatenate: %s.\n", str);
    assert(equals(str, "String1String2"));
    free(str);

    return 0;
}
