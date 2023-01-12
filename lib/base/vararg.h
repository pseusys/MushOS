#ifndef MUSHLIB_VARARG_H
#define MUSHLIB_VARARG_H

#include "heap.h"

// TODO: check stack base with error!
// TODO: setup different arg types
#define args_init_from(args, skip) {\
    u_dword stack_offset = 8 + skip;\
    for (int i = 0; i < size_of(args); ++i) {\
        u_dword arg;\
        asm volatile(\
            "add %%ebp, %1\n"\
            "mov (%1), %0"\
            : "=r"(arg): "r"(stack_offset)\
        );\
        args[i] = arg;\
        stack_offset += sizeof(u_dword);\
    }\
}

#define args_init(args) {\
    args_init_from(args, 0);\
}

void* get_args(u_dword num, ...);

#endif // MUSHLIB_VARARG_H
