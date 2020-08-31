#include "vararg.h"


void* get_args(u_dword num, ...) {
    u_dword* args = malloc(num * sizeof(u_dword) + 1);
    args_init_from(args, sizeof(u_dword))
    return args;
}
