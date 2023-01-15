#ifndef MUSHLIB_SYSCALL_H
#define MUSHLIB_SYSCALL_H

#include "generic.h"
#include "vararg.h"


#define call_system(interruption, args_num, arguments...) {\
    asm volatile ("pusha");\
    u_dword* args = get_args(args_num, ## arguments);\
    for (int i = args_num - 1; i >= 0; i--) asm volatile ("push %0" :: "r"(args[i]));\
    unalloc(args);\
    asm volatile (\
        "push %%ebp\n"\
        "mov %%esp, %%ebp\n"\
        "int %0\n"\
        "mov %%ebp, %%esp\n"\
        "pop %%ebp" :: "i"(interruption)\
    );\
    for (int i = args_num - 1; i >= 0; i--) asm volatile ("pop %eax");\
    asm volatile ("popa");\
}

#define call_system_ret(interruption, ret_value, args_num, arguments...) {\
    asm volatile ("pusha");\
    u_dword* args = get_args(args_num, ## arguments);\
    for (int i = args_num - 1; i >= 0; i--) asm volatile ("push %0" :: "r"(args[i]));\
    unalloc(args);\
    asm volatile (\
        "push %%ebp\n"\
        "mov %%esp, %%ebp\n"\
        "int %0\n"\
        "mov %%ebp, %%esp\n"\
        "pop %%ebp" :: "i"(interruption)\
    );\
    for (int i = args_num - 1; i >= 0; i--) asm volatile ("pop %0" : "=r"(ret_value));\
    asm volatile ("popa");\
}



#define system_get_arg(stack_base, arg_num, ret_val)\
    asm volatile ("mov (%1), %0" : "=r" (ret_val) : "r" (stack_base + 4 + arg_num * sizeof(u_dword)));

#define system_push(stack_base, value) {\
    u_dword position = stack_base + 4;\
    asm volatile ("mov %1, (%0)" :: "r" (position), "r" (value));\
}

#endif // MUSHLIB_SYSCALL_H
