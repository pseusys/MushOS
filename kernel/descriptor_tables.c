#include "descriptor_tables.h"
#include "../mushlib/memory.h"
#include "../drivers/screen.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void idt_flush(u_dword GDT_descriptor_pos);

IDT_entry* idt = nullptr;
IDT_descriptor* idt_descriptor_pointer = nullptr;

IDT_entry idt_TEMP [256];
IDT_descriptor idt_descriptor_pointer_TEMP;

void init_interruptions() {
    memory_clear((byte *) idt_TEMP, 256 * sizeof(IDT_entry), 0);

    u_word CS = 0x08;
    idt_TEMP[0] = create_idt_entry((u_dword) isr0, CS, 0b10001110);
    idt_TEMP[1] = create_idt_entry((u_dword) isr1, CS, 0b10001110);
    idt_TEMP[2] = create_idt_entry((u_dword) isr2, CS, 0b10001110);
    idt_TEMP[3] = create_idt_entry((u_dword) isr3, CS, 0b10001110);
    idt_TEMP[4] = create_idt_entry((u_dword) isr4, CS, 0b10001110);
    idt_TEMP[5] = create_idt_entry((u_dword) isr5, CS, 0b10001110);
    idt_TEMP[6] = create_idt_entry((u_dword) isr6, CS, 0b10001110);
    idt_TEMP[7] = create_idt_entry((u_dword) isr7, CS, 0b10001110);
    idt_TEMP[8] = create_idt_entry((u_dword) isr8, CS, 0b10001110);
    idt_TEMP[9] = create_idt_entry((u_dword) isr9, CS, 0b10001110);
    idt_TEMP[10] = create_idt_entry((u_dword) isr10, CS, 0b10001110);
    idt_TEMP[11] = create_idt_entry((u_dword) isr11, CS, 0b10001110);
    idt_TEMP[12] = create_idt_entry((u_dword) isr12, CS, 0b10001110);
    idt_TEMP[13] = create_idt_entry((u_dword) isr13, CS, 0b10001110);
    idt_TEMP[14] = create_idt_entry((u_dword) isr14, CS, 0b10001110);
    idt_TEMP[15] = create_idt_entry((u_dword) isr15, CS, 0b10001110);
    idt_TEMP[16] = create_idt_entry((u_dword) isr16, CS, 0b10001110);
    idt_TEMP[17] = create_idt_entry((u_dword) isr17, CS, 0b10001110);
    idt_TEMP[18] = create_idt_entry((u_dword) isr18, CS, 0b10001110);
    idt_TEMP[19] = create_idt_entry((u_dword) isr19, CS, 0b10001110);
    idt_TEMP[20] = create_idt_entry((u_dword) isr20, CS, 0b10001110);
    idt_TEMP[21] = create_idt_entry((u_dword) isr21, CS, 0b10001110);
    idt_TEMP[22] = create_idt_entry((u_dword) isr22, CS, 0b10001110);
    idt_TEMP[23] = create_idt_entry((u_dword) isr23, CS, 0b10001110);
    idt_TEMP[24] = create_idt_entry((u_dword) isr24, CS, 0b10001110);
    idt_TEMP[25] = create_idt_entry((u_dword) isr25, CS, 0b10001110);
    idt_TEMP[26] = create_idt_entry((u_dword) isr26, CS, 0b10001110);
    idt_TEMP[27] = create_idt_entry((u_dword) isr27, CS, 0b10001110);
    idt_TEMP[28] = create_idt_entry((u_dword) isr28, CS, 0b10001110);
    idt_TEMP[29] = create_idt_entry((u_dword) isr29, CS, 0b10001110);
    idt_TEMP[30] = create_idt_entry((u_dword) isr30, CS, 0b10001110);
    idt_TEMP[31] = create_idt_entry((u_dword) isr31, CS, 0b10001110);

    idt_descriptor_pointer_TEMP.size = 256 - 1;
    idt_descriptor_pointer_TEMP.pos = (u_dword) idt_TEMP;

    print("Interruptions loaded.\n");
    set_idt(idt_TEMP);
}

/*void set_idt (IDT_entry* entries) {
    u_dword size = 256;

    IDT_descriptor* new_IDT_d_p = (IDT_descriptor*) allocate_memory(1, sizeof(IDT_descriptor));
    IDT_entry* new_IDTp = (IDT_entry*) allocate_memory(size, sizeof(IDT_entry));

    new_IDT_d_p->size = (sizeof(IDT_entry) * size) - 1;
    new_IDT_d_p->pos  = (u_dword) new_IDTp;

    memory_copy((byte*) entries, (byte*) new_IDTp, new_IDT_d_p->size + 1);
    idt_flush((u_dword) new_IDT_d_p);

    free_memory((byte *) idt);
    free_memory((byte *) idt_descriptor_pointer);
    idt = new_IDTp;
    idt_descriptor_pointer = new_IDT_d_p;
}*/

void set_idt (IDT_entry* entries) {
    idt_flush((u_dword) &idt_descriptor_pointer_TEMP);
}

IDT_entry create_idt_entry (u_dword base, u_word selector, u_byte flags) {
    IDT_entry entry;
    entry.base_low = (base & 0xffff);
    entry.selector = selector;
    entry.always0 = 0x00;
    entry.flags = flags;
    entry.base_high = (base >> 16);
    return entry;
}




void isr_handler(registers regs) {
    print("recieved interrupt: ");
    print_char(regs.int_no + 0x30);
    print_char('\n');
}
