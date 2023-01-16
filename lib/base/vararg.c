#include "vararg.h"


void* extract_dword_args(u_dword args_num, ...) {
    u_dword* args = ralloc(args_num * sizeof(u_dword));
    for (int i = 0; i < args_num; ++i)
        args[i] = get_arg(sizeof(u_dword) * (i + 1), u_dword);
    return args;
}
