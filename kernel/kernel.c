#include "../drivers/screen.h"
#include "descriptor_tables.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"

/**
 * Make kernel constants:
 * Root CS and root DS (passed from loader maybe).
 * Kernel size (written in boot sector, defined after compilation).
 *
 * Divide boot loader into: loader, functions, GDT (final!).
 *
 * First man 2 implement: https://web.archive.org/web/20160324232118/http://jamesmolloy.co.uk/tutorial_html/
 * Second man 2 implement: https://github.com/cfenollosa/os-tutorial
 *
 * MH! Logger functions + specified data types (string, arrays, stack, line).
 * 0. Audio drivers.
 * 1. Memory functions.
 * 2. Shell.
 * 3. File system.
 * 4. GDT + positioning of kernel / stack + kernel loading.
 * 5. File system.
 * 6. Heap.
 * 7. Multitasking.
 * 8. User mode.
 * 9. Video mode.
 * 10. Sample apps.
 * 11. Und so weiter...
 */

int kek() {
    char* video_memory = (char*) 0xb8000;
    video_memory[0] = 'A';
    return 0;
}

void _start() {
    clear_screen();
    print("Kernel started!\n");
    init_interruptions();
    init_keyboard();
    init_timer(100);
}
