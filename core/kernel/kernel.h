#ifndef KERNEL_H
#define KERNEL_H

#include "../../lib/base/exceptions.h"


#define PANIC(message) {\
    throw_verbose(0, "KERNEL PANIC", message)\
    asm("jmp .");\
}

#endif // KERNEL_H
