#include "../../../MushLib/heap.h"
#include "../../../MushLib/vararg.h"

#define orbital_slot 0x100000


char* silos = "Beep-beep\n";

void _start() {
    //initialize_heap((void*) orbital_slot, 0x3000);

    asm volatile ("push %0" :: "r"(0));
    asm volatile ("push %0" :: "r"(0));
    asm volatile ("push %0" :: "r"(15));
    asm volatile ("push %0" :: "r"(silos));
    asm volatile ("push %0" :: "r"(2));
    asm volatile (
        "push %%ebp\n"
        "mov %%esp, %%ebp\n"
        "int %0\n"
        "mov %%ebp, %%esp\n"
        "pop %%ebp" :: "i"(48)
    );
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
}
