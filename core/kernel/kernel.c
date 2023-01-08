#include "kernel.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/heap.h"
#include "../../lib/base/stdio.h"

#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "interruption_tables.h"
#include "timer.h"
#include "pages.h"
#include "modules.h"
#include "pages.h"


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
    initialize_heap((void*) 0x500, 0x6000);

    init_interruptions();
    init_timer(100);
    init_screen_io();

    init_debug_handler();
    good("Kernel started, build: %s - %s\n", __DATE__, __TIME__)

    init_keyboard();
    init_module_loading_driver();

    initialise_paging();

    PANIC("Oh no! I'm in panic!!")
    u_dword *ptr = (u_dword*) 0xA0000000;
    u_dword do_page_fault = *ptr;
}
