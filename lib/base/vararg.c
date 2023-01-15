#include "vararg.h"


void* get_args(u_dword num, ...) {
    u_dword* args = ralloc(num * sizeof(u_dword));
    args_init_from(args, sizeof(u_dword))
    return args;
}
