#ifndef MUSHLIB_VARARG_H
#define MUSHLIB_VARARG_H

#include "heap.h"
#include "math.h"


#define init_vararg(skip) ({\
    u_dword offset = skip & 0xfffffffc;\
    if (skip % 4 != 0) offset += 4;\
    offset;\
})

#define get_vararg(offset, type) ({\
    type result = get_arg(offset, type);\
    offset += max(sizeof(type), 4);\
    result;\
})


// TODO: check stack base with error!
#define get_arg(skip, type) ({\
    type* arg;\
    asm volatile(\
        "add %%ebp, %1\n"\
        "mov %1, %0"\
        : "=r"(arg): "r"(8 + skip)\
    );\
    *arg;\
})

void* extract_dword_args(u_dword args_num, ...);

#endif // MUSHLIB_VARARG_H
