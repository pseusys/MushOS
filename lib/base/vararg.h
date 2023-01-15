#ifndef MUSHLIB_VARARG_H
#define MUSHLIB_VARARG_H

#include "heap.h"
#include "math.h"


#define init_vararg(skip) ({\
    skip;\
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

#define args_init_from(args, skip) {\
    for (u_dword i = 0; i < size(args); ++i)\
        *(args + i) = get_arg(skip + sizeof(u_byte) * i, u_byte);\
}

void* get_args(u_dword num, ...);

#endif // MUSHLIB_VARARG_H
