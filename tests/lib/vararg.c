#include <mushlib.h>

#include <assert.h>
#include <stdio.h>

#include "../integrator.h"


void vararg_integers_printing_function(u_dword arg_number, ...) {
    printf("Function call with %d integers:\n", arg_number);
    for (u_dword i = 0; i < arg_number; i++) {
        u_dword arg = get_arg(sizeof(u_dword) * (i + 1), u_dword);
        printf("    argument #%d: %x.\n", i, arg);
        assert(arg == i + 1);
    }
}

void vararg_int_str_float_by_one_function(u_dword _, ...) {
    printf("Function call with 3 divverent values:\n");

    u_dword arg1 = get_arg(sizeof(u_dword), u_qword);
    printf("    u_qword argument: %x.\n", arg1);
    assert(arg1 == 1);

    string arg2 = get_arg(sizeof(u_dword) + sizeof(u_qword), string);
    printf("    string argument: '%s'.\n", arg2);
    assert(arg2 == "Some string");

    precise arg3 = get_arg(sizeof(u_dword) + sizeof(u_qword) + sizeof(string), precise);
    printf("    precise argument: %f.\n", arg3);
    assert(arg3 == 3.1415);
}

void vararg_3_different_long_printing_function(u_dword _, ...) {
    u_dword vararg_offset = init_vararg(sizeof(u_dword));
    printf("Function call with 3 different longs (offset %d):\n", vararg_offset);

    u_word arg1 = get_vararg(vararg_offset, u_word);
    printf("    u_word argument (offset %d): %x.\n", vararg_offset, arg1);
    assert(arg1 == 1);

    u_dword arg2 = get_vararg(vararg_offset, u_dword);
    printf("    u_dword argument (offset %d): %x.\n", vararg_offset, arg2);
    assert(arg2 == 2);

    u_qword arg3 = get_vararg(vararg_offset, u_qword);
    printf("    u_qword argument (offset %d): %x.\n", vararg_offset, arg3);
    assert(arg3 == 3);
}


int main() {
    void* heap = allocate_space_for_heap(heap_size);
    initialize_heap(heap, heap_size);

    vararg_integers_printing_function(1, (u_dword) 1);
    vararg_integers_printing_function(2, (u_dword) 1, (u_dword) 2);
    vararg_integers_printing_function(3, (u_dword) 1, (u_dword) 2, (u_dword) 3);

    vararg_int_str_float_by_one_function(3, (u_qword) 1, "Some string", (precise) 3.1415);

    vararg_3_different_long_printing_function(3, (u_word) 1, (u_dword) 2, (u_qword) 3);

    check_heap_for_leaks();
    return 0;
}
