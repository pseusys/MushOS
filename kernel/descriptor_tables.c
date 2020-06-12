#include "descriptor_tables.h"
#include "../mushlib/memory.h"

/*OBSOLETE extern void gdt_flush(u_dword GDT_descriptor_pos, u_dword current_code, u_dword current_data);

static GDT_entry* gdt_pointer = nullptr;
static GDT_descriptor* gdt_descriptor_pointer = nullptr;

void init_general() {
    GDT_entry GDT [3];
    GDT[0] = create_gdt_entry(0x0000, 0x00000000, 0x00, 0x00);
    GDT[1] = create_gdt_entry(0xffff, 0x00000000, 0b10011010, 0b11001111);
    GDT[2] = create_gdt_entry(0xffff, 0x00000000, 0b10010010, 0b11001111);
    set_gdt(GDT, 3);
}

void set_gdt (GDT_entry* entries, u_word entries_num) {
    GDT_descriptor* new_GDT_d_p = (GDT_descriptor*) allocate_memory(1, sizeof(GDT_descriptor));
    GDT_entry* new_GDTp = (GDT_entry*) allocate_memory(entries_num, sizeof(GDT_entry));

    new_GDT_d_p->size = (sizeof(GDT_entry) * entries_num) - 1;
    new_GDT_d_p->pos  = (u_dword) new_GDTp;

    memory_copy((byte*) entries, (byte*) new_GDTp, new_GDT_d_p->size + 1);
    gdt_flush((u_dword) new_GDT_d_p, 0x8, 0x10); //TODO: implement user segments.

    free_memory((byte *) gdt_pointer);
    free_memory((byte *) gdt_descriptor_pointer);
    gdt_pointer = new_GDTp;
    gdt_descriptor_pointer = new_GDT_d_p;
}

GDT_entry create_gdt_entry (u_word limit, u_dword base, byte flags1, byte flags2) {
    GDT_entry entry;
    entry.limit = limit;
    entry.base_low = (base & 0xffff);
    entry.base_middle = ((base >> 16) & 0xff);
    entry.flags_low = flags1;
    entry.flags_high = flags2;
    entry.base_high = (base >> 24);
    return entry;
}*/



/*extern u_word get_cs();

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

static IDT_entry* idt = nullptr;
static IDT_descriptor* idt_descriptor_pointer = nullptr;

void init_interruptions() {
    IDT_entry IDT [256];
    memory_clear((byte *) IDT, 256 * sizeof(IDT_entry), 0);

    u_word CS = get_cs();
    IDT[0] = create_idt_entry((u_dword) isr0, CS, 0b10001110);
    IDT[1] = create_idt_entry((u_dword) isr1, CS, 0b10001110);
    IDT[2] = create_idt_entry((u_dword) isr2, CS, 0b10001110);
    IDT[3] = create_idt_entry((u_dword) isr3, CS, 0b10001110);
    IDT[4] = create_idt_entry((u_dword) isr4, CS, 0b10001110);
    IDT[5] = create_idt_entry((u_dword) isr5, CS, 0b10001110);
    IDT[6] = create_idt_entry((u_dword) isr6, CS, 0b10001110);
    IDT[7] = create_idt_entry((u_dword) isr7, CS, 0b10001110);
    IDT[8] = create_idt_entry((u_dword) isr8, CS, 0b10001110);
    IDT[9] = create_idt_entry((u_dword) isr9, CS, 0b10001110);
    IDT[10] = create_idt_entry((u_dword) isr10, CS, 0b10001110);
    IDT[11] = create_idt_entry((u_dword) isr11, CS, 0b10001110);
    IDT[12] = create_idt_entry((u_dword) isr12, CS, 0b10001110);
    IDT[13] = create_idt_entry((u_dword) isr13, CS, 0b10001110);
    IDT[14] = create_idt_entry((u_dword) isr14, CS, 0b10001110);
    IDT[15] = create_idt_entry((u_dword) isr15, CS, 0b10001110);
    IDT[16] = create_idt_entry((u_dword) isr16, CS, 0b10001110);
    IDT[17] = create_idt_entry((u_dword) isr17, CS, 0b10001110);
    IDT[18] = create_idt_entry((u_dword) isr18, CS, 0b10001110);
    IDT[19] = create_idt_entry((u_dword) isr19, CS, 0b10001110);
    IDT[20] = create_idt_entry((u_dword) isr20, CS, 0b10001110);
    IDT[21] = create_idt_entry((u_dword) isr21, CS, 0b10001110);
    IDT[22] = create_idt_entry((u_dword) isr22, CS, 0b10001110);
    IDT[23] = create_idt_entry((u_dword) isr23, CS, 0b10001110);
    IDT[24] = create_idt_entry((u_dword) isr24, CS, 0b10001110);
    IDT[25] = create_idt_entry((u_dword) isr25, CS, 0b10001110);
    IDT[26] = create_idt_entry((u_dword) isr26, CS, 0b10001110);
    IDT[27] = create_idt_entry((u_dword) isr27, CS, 0b10001110);
    IDT[28] = create_idt_entry((u_dword) isr28, CS, 0b10001110);
    IDT[29] = create_idt_entry((u_dword) isr29, CS, 0b10001110);
    IDT[30] = create_idt_entry((u_dword) isr30, CS, 0b10001110);
    IDT[31] = create_idt_entry((u_dword) isr31, CS, 0b10001110);

    set_idt(IDT);
}

void set_idt (IDT_entry* entries) {
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




void isr_handler(registers regs)
{
    print("recieved interrupt: ");
    print_char(regs.int_no + 0x30);
    print_char('\n');
}*/
