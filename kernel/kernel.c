#include "../drivers/screen.h"
#include "descriptor_tables.h"

/**
 * Make kernel constants:
 * Root CS and root DS (passed from loader maybe).
 * Kernel size (written in boot sector, defined after compilation).
 *
 * Divide boot loader into: loader, functions, GDT (final!).
 */

int kek() {
    char* video_memory = (char*) 0xb8000;
    video_memory[0] = 'A';
    return 0;
}

void _start() {
    //clear_screen();
    init_interruptions();
    print("LOL\n");
    asm volatile ("int $0x3");
}
