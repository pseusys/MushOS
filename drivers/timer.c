#include "timer.h"
#include "../kernel/interruption_tables.h"
#include "../kernel/interruptions.h"
#include "screen.h"
#include "ports_io.h"

static u_dword tick = 0;

static void timer_callback(registers* regs) {
    tick++;
    /*print("Tick: ");
    print_char(tick + 0x30); // TODO: num to str.
    print_char('\n');*/
}

void init_timer(u_dword frequency) {
    // Firstly, register our timer callback.
    set_interrupt_handler(IRQ0, &timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    u_dword divisor = 1193180 / frequency;

    // Send the command byte.
    port_byte_out(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    u_byte l = (u_byte)(divisor & 0xFF);
    u_byte h = (u_byte)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    port_byte_out(0x40, l);
    port_byte_out(0x40, h);
}
