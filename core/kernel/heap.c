#include "../../lib/base/heap.h"
#include "../../lib/base/stdio.h"
#include "../../lib/base/exceptions.h"


extern void print_string_color(string str, byte text_color, byte back_color, u_dword length);


static void handle_allocation_exception() {
    print_string_color("Kernel heap exhausted!\n", HIGH_RED, BLACK, 0);
    asm volatile ("jmp .");
}

extern void initialize_kernel_heap_handler() {
    handle_exceptions(allocation_exception_id, handle_allocation_exception);
}
