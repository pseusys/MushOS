#ifndef MUSHOS_SYSCALL_H
#define MUSHOS_SYSCALL_H

#include "../kernel/generic.h"
#include "stdio.h"
#include "vararg.h"


#define orbit(interruption, args_num, arguments...) {\
    u_dword* args = get_args(args_num, ## arguments);\
    for (int i = 0; i < args_num; ++i) asm volatile ("push %0" :: "r"(args[i]));\
    free(args);\
    asm volatile (\
        "push %%ebp\n"\
        "mov %%esp, %%ebp\n"\
        "int %0\n"\
        "mov %%ebp, %%esp\n"\
        "pop %%ebp" :: "i"(interruption)\
    );\
}

#define orbit_ret(interruption, ret_value, args_num, arguments...) {\
    orbit(interruption, args_num, ## arguments)\
    for (int i = 0; i < args_num - 1; ++i) {\
        asm volatile ("pop %0" : "=r"(ret_value));\
    }\
    asm volatile ("pop %0" : "=r"(ret_value));\
}



#define get_orbital_arg(stack_base, arg_num, ret_val)\
    asm volatile ("mov (%1), %0\n" : "=r" (ret_val) : "r" (stack_base + 4 + arg_num * sizeof(u_dword)));

#define push_orbital(stack_base, value) {\
    u_dword position = stack_base + 4;\
    asm volatile ("mov %1, (%0)\n" :: "r" (position), "r" (value));\
}

#endif //MUSHOS_SYSCALL_H
