#include <../../lib/base/vararg.h>

#include <assert.h>
#include <stdio.h>

#include "../integrator.h"


void vararg_integers_printing_function(u_dword arg_number, ...) {
    u_byte* args = (u_byte*) malloc(arg_number * sizeof(u_dword));
    args_init_from(args, sizeof(u_dword));
    printf("Function call with %d integers:\n", arg_number);
    for (u_dword i = 0; i < arg_number; i++) printf("    argument #%d: %x.\n", i, ((u_dword*) args)[i]);
}

void vararg_3_different_long_printing_function(u_dword _, ...) {
    u_byte* args = (u_byte*) malloc(sizeof(u_dword) + sizeof(u_qword) + sizeof(u_xword));
    args_init_from(args, sizeof(u_dword));
    printf("Function call with 3 different longs:\n");
    printf("    u_dword argument: %x.\n", *((u_dword*) args));
    printf("    u_qword argument: %x.\n", *((u_qword*) (args + sizeof(u_dword))));
    printf("    u_xword argument: %x.\n", *((u_xword*) (args + sizeof(u_dword) + sizeof(u_qword))));
}

void vararg_int_str_float_by_one_function(u_dword _, ...) {
    printf("Function call with 3 divverent values:\n");
    printf("    u_qword argument: %x.\n", get_arg(sizeof(u_dword), u_qword));
    printf("    string argument: '%s'.\n", get_arg(sizeof(u_dword) + sizeof(u_dword), string));
    printf("    precise argument: %f.\n", get_arg(sizeof(u_dword) + sizeof(u_qword) + sizeof(string), precise));
}


int main() {
    void* heap = allocate_space_for_heap(heap_size);
    initialize_heap(heap, heap_size);

    vararg_integers_printing_function(1, (u_dword) 1);
    vararg_integers_printing_function(2, (u_dword) 1, (u_dword) 2);
    vararg_integers_printing_function(3, (u_dword) 1, (u_dword) 2, (u_dword) 3);

    vararg_3_different_long_printing_function(3, (u_dword) 1, (u_qword) 2, (u_xword) 3);

    vararg_int_str_float_by_one_function(3, (u_qword) 1, "Some string", (precise) 3.1415);

    return 0;
}
