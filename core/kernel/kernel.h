#ifndef KERNEL_H
#define KERNEL_H

#include "../../lib/base/exceptions.h"
#include "../../lib/base/string.h"


#define PANIC(message, args...) {\
    mod_string formatted = format(message, ## args);\
    throw_verbose(0, "KERNEL PANIC", formatted)\
    asm volatile ("jmp .");\
}

#endif // KERNEL_H
