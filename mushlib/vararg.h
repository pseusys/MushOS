#ifndef MUSHOS_VARARG_H
#define MUSHOS_VARARG_H

#include "heap.h"

#define args_init(args) {\
    u_dword stack_offset = 8;\
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

#endif //MUSHOS_VARARG_H
