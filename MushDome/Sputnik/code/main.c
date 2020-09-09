#include "../../../MushLib/heap.h"
#include "../../../MushLib/vararg.h"

#define orbital_slot 0x100000


char* silos = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean tempor consequat velit id rutrum. Donec congue pulvinar blandit. Nunc vestibulum vulputate facilisis. Aliquam pharetra malesuada sodales. Cras porta, arcu ac dignissim tincidunt, turpis risus egestas nunc, ac accumsan tellus velit eget mi. Curabitur id ornare purus, at hendrerit lorem. Donec fermentum eget turpis sit amet vulputate.\n"
              "\n"
              "Aliquam erat volutpat. Etiam pulvinar lacus ut elementum porta. Proin sodales ante quis felis pellentesque semper. Praesent auctor turpis arcu, in condimentum sem auctor tincidunt. Quisque ultricies purus ullamcorper diam rhoncus, eget eleifend purus tristique. Proin congue pretium feugiat. Nullam massa magna, vestibulum sit amet dui ac, dictum gravida turpis.\n"
              "\n"
              "Etiam iaculis, odio sed dictum molestie, erat sapien elementum purus, eu consectetur mi odio id metus. Sed a odio ac sapien egestas maximus mollis in est. Duis sollicitudin mollis est nec aliquam. Sed hendrerit tincidunt hendrerit. Sed non.";

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
