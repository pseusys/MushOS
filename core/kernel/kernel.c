/**
 * @file
 * 
 * @brief Kernel entry point
 */

#include "kernel.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/heap.h"
#include "../../lib/base/stdio.h"
#include "../../lib/base/exceptions.h"
#include "../../lib/base/string.h"

#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "interruption_tables.h"
#include "timer.h"
#include "pages.h"
#include "task.h"
#include "modules.h"

extern void initialize_kernel_heap_handler();


#define kernel_heap_start 0x50000
#define kernel_heap_size 0x80000

#define kernel_timer_step 100

void* stack_pointer = (void*) 0x7000;
u_dword stack_size = 0x5000;

/**
 * Make kernel constants:
 *
 *
 * Divide boot loader into: loader, functions, GDT.
 *
 * First man: https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
 * Second man: https://web.archive.org/web/20160324232118/http://jamesmolloy.co.uk/tutorial_html/
 * Third man: https://github.com/cfenollosa/os-tutorial
 *
 * Documentation! For each stage!
 * 1. Kernel constants (root CS and root DS (passed from loader), booting device, kernel size, max RAM)
 *                          -> define RAM positions everywhere (loader, interruptions).
 * 2. Memory map -> positioning of kernel / stack + kernel loading size (written in boot sector, defined after compilation).
 * 3. Shell + audio drivers.
 * 4. File system + initrd.
 * 6. Multitasking.
 * 7. User mode.
 * 8. Video mode.
 * 9. Sample apps.
 * 10. Und so weiter...
 * Extra! Add testing in Travis CI with special QEMU target in console mode.
 */

void _start() {
    initialize_heap((void*) kernel_heap_start, kernel_heap_size);
    initialize_kernel_heap_handler();

    init_interruptions();
    init_timer(kernel_timer_step);
    init_screen_io();

    init_debug_handler();
    good("Kernel started, build: %s - %s\n", __DATE__, __TIME__)

    init_keyboard();
    init_module_loading_driver();

    initialise_paging();

    initialise_tasking();

    // Create a new process in a new address space which is a clone of this.
    int ret = fork();

    info("fork() returned %d, and getpid() returned %d\n", ret, getpid())
}
