#ifndef KERNEL_H
#define KERNEL_H

#include <mushlib.h>

#define PANIC(message) {\
    print_colored(HIGH_RED, BLACK, message);\
    while (true) {}\
}

#endif // KERNEL_H
