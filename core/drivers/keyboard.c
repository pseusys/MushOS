#include "keyboard.h"

#include "../kernel/interruption_tables.h"
#include "../kernel/interruptions.h"

#include "ports_io.h"


static u_byte RECOGNIZED_CODES [] = {0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                                     0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
                                     0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1e, 0x1f,
                                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                     0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
                                     0x35, 0x39};
static u_byte RECOGNIZED_CHARS [] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
                                     '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
                                     'O', 'P', '[', ']', '\n', 'A', 'S', 'D', 'F', 'G', 'H',
                                     'J', 'K', 'L', ';', '\'', '`', '\\', 'Z', 'X', 'C', 'V',
                                     'B', 'N', 'M', ',', '.', '/', ' '};

static void print_letter(u_byte scancode) {
    for (int i = 0; i < 51; ++i) {
        //if (scancode == RECOGNIZED_CODES[i]) print_char(RECOGNIZED_CHARS[i]);
    }
    if (scancode <= 0x7f); // Unknown key up.
    else; // Unknown key down.
}

static void keyboard_callback(registers* regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    u_byte scancode = port_byte_in(0x60);
    //print("Keyboard input: ");
    print_letter(scancode);
    //print_char('\n');
}

void init_keyboard() {
    set_interrupt_handler(IRQ1, keyboard_callback);
}
