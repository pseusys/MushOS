#ifndef KERNEL_H
#define KERNEL_H

#include "../../lib/base/syscall.h"


#define PANIC(message) {\
    call_orbit(48, 5, 2, message, HIGH_RED, BLACK, 0)\
    while (true) {}\
}

#endif // KERNEL_H
