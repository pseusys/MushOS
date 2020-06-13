#include "interruption_tables.h"
#include "../mushlib/memory.h"
#include "../drivers/screen.h"
#include "../drivers/ports_io.h"
#include "interruptions.h"

IDT_entry idt [256];
IDT_descriptor idt_descriptor_pointer;

interruption_handler interruption_handlers[256];

static void create_idt_entry (u_dword pos, u_dword base, u_word selector, u_byte flags) {
    idt[pos].base_low = (base & 0xffff);
    idt[pos].selector = selector;
    idt[pos].always0 = 0x00;
    idt[pos].flags = flags;
    idt[pos].base_high = (base >> 16);
}

void init_interruptions() {
    memory_clear((byte *) idt, 256 * sizeof(IDT_entry), 0);

    u_word CS = 0x08;
    create_idt_entry(0, (u_dword) isr0, CS, 0b10001110);
    create_idt_entry(1, (u_dword) isr1, CS, 0b10001110);
    create_idt_entry(2, (u_dword) isr2, CS, 0b10001110);
    create_idt_entry(3, (u_dword) isr3, CS, 0b10001110);
    create_idt_entry(4, (u_dword) isr4, CS, 0b10001110);
    create_idt_entry(5, (u_dword) isr5, CS, 0b10001110);
    create_idt_entry(6, (u_dword) isr6, CS, 0b10001110);
    create_idt_entry(7, (u_dword) isr7, CS, 0b10001110);
    create_idt_entry(8, (u_dword) isr8, CS, 0b10001110);
    create_idt_entry(9, (u_dword) isr9, CS, 0b10001110);
    create_idt_entry(10, (u_dword) isr10, CS, 0b10001110);
    create_idt_entry(11, (u_dword) isr11, CS, 0b10001110);
    create_idt_entry(12, (u_dword) isr12, CS, 0b10001110);
    create_idt_entry(13, (u_dword) isr13, CS, 0b10001110);
    create_idt_entry(14, (u_dword) isr14, CS, 0b10001110);
    create_idt_entry(15, (u_dword) isr15, CS, 0b10001110);
    create_idt_entry(16, (u_dword) isr16, CS, 0b10001110);
    create_idt_entry(17, (u_dword) isr17, CS, 0b10001110);
    create_idt_entry(18, (u_dword) isr18, CS, 0b10001110);
    create_idt_entry(19, (u_dword) isr19, CS, 0b10001110);
    create_idt_entry(20, (u_dword) isr20, CS, 0b10001110);
    create_idt_entry(21, (u_dword) isr21, CS, 0b10001110);
    create_idt_entry(22, (u_dword) isr22, CS, 0b10001110);
    create_idt_entry(23, (u_dword) isr23, CS, 0b10001110);
    create_idt_entry(24, (u_dword) isr24, CS, 0b10001110);
    create_idt_entry(25, (u_dword) isr25, CS, 0b10001110);
    create_idt_entry(26, (u_dword) isr26, CS, 0b10001110);
    create_idt_entry(27, (u_dword) isr27, CS, 0b10001110);
    create_idt_entry(28, (u_dword) isr28, CS, 0b10001110);
    create_idt_entry(29, (u_dword) isr29, CS, 0b10001110);
    create_idt_entry(30, (u_dword) isr30, CS, 0b10001110);
    create_idt_entry(31, (u_dword) isr31, CS, 0b10001110);

    u_byte master_mask = port_byte_in(0x21);
    u_byte slave_mask = port_byte_in(0xA1);
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);
    port_byte_out(0x21, master_mask);
    port_byte_out(0xA1, slave_mask);

    create_idt_entry(32, (u_dword) irq0, CS, 0b10001110);
    create_idt_entry(33, (u_dword) irq1, CS, 0b10001110);
    create_idt_entry(34, (u_dword) irq2, CS, 0b10001110);
    create_idt_entry(35, (u_dword) irq3, CS, 0b10001110);
    create_idt_entry(36, (u_dword) irq4, CS, 0b10001110);
    create_idt_entry(37, (u_dword) irq5, CS, 0b10001110);
    create_idt_entry(38, (u_dword) irq6, CS, 0b10001110);
    create_idt_entry(39, (u_dword) irq7, CS, 0b10001110);
    create_idt_entry(40, (u_dword) irq8, CS, 0b10001110);
    create_idt_entry(41, (u_dword) irq9, CS, 0b10001110);
    create_idt_entry(42, (u_dword) irq10, CS, 0b10001110);
    create_idt_entry(43, (u_dword) irq11, CS, 0b10001110);
    create_idt_entry(44, (u_dword) irq12, CS, 0b10001110);
    create_idt_entry(45, (u_dword) irq13, CS, 0b10001110);
    create_idt_entry(46, (u_dword) irq14, CS, 0b10001110);
    create_idt_entry(47, (u_dword) irq15, CS, 0b10001110);

    for (int i = 48; i < 256; ++i) {
        create_idt_entry(i, (u_dword) isr32, CS, 0b10001110);
    }

    idt_descriptor_pointer.size = 256 * sizeof(IDT_entry) - 1;
    idt_descriptor_pointer.pos = (u_dword) &idt;

    idt_flush((u_dword) &idt_descriptor_pointer);
}



void set_interrupt_handler(u_byte n, interruption_handler handler) {
    print("Handler set for interruption: ");
    print_char(n + 0x30); // TODO: num to str.
    print_char('\n');
    interruption_handlers[n] = handler;
}



void isr_handler(registers* regs) {
    if (regs->int_no == DEFAULT_IRS) print_color("Received undefined user interrupt.\n", HIGH_RED, BLACK);

    if (interruption_handlers[regs->int_no] != nullptr) {
        interruption_handler handler = interruption_handlers[regs->int_no];
        handler(regs);
    } else {
        print_color("Received processor interrupt: ", HIGH_RED, BLACK);
        print_char_color(regs->int_no + 0x30, HIGH_RED, BLACK); // TODO: add definition to all 32 ints.
        print_char_color('\n', HIGH_RED, BLACK);
    }
}

void irq_handler(registers* regs) {
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.

    if (regs->int_no >= 40) {
        // Send reset signal to slave.
        port_byte_out(0xA0, 0x20);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    port_byte_out(0x20, 0x20);

    if (interruption_handlers[regs->int_no] != nullptr) {
        interruption_handler handler = interruption_handlers[regs->int_no];
        handler(regs);
    } else {
        print_color("Received hardware interrupt: ", HIGH_BLUE, BLACK);
        print_char_color(regs->int_no + 0x30, HIGH_BLUE, BLACK); // TODO: add definition to all 16 ints.
        print_char_color('\n', HIGH_BLUE, BLACK);
    }
}