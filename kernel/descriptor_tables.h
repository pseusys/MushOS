#ifndef MUSHOS_DESCRIPTOR_TABLES_H
#define MUSHOS_DESCRIPTOR_TABLES_H

/*#include "generic.h"

typedef struct {
    u_word limit;
    u_word base_low;
    byte base_middle;
    byte flags_low;
    byte flags_high;
    byte base_high;
} __attribute__((packed)) GDT_entry;

typedef struct {
    u_word size;
    u_dword pos;
} __attribute__((packed)) GDT_descriptor;

OBSOLETE void init_general();
OBSOLETE GDT_entry create_gdt_entry (u_word limit, u_dword base, byte flags1, byte flags2);
OBSOLETE void set_gdt (GDT_entry* entries, u_word entries_num);*/



/*typedef struct {
    u_word base_low;
    u_word selector;
    u_byte always0;
    u_byte flags;
    u_word base_high;
} __attribute__((packed)) IDT_entry;

typedef struct {
    u_word size;
    u_dword pos;
} __attribute__((packed)) IDT_descriptor;

void init_interruptions();
void set_idt(IDT_entry* entries);
IDT_entry create_idt_entry(u_dword base, u_word selector, u_byte flags);



typedef struct {
    u_dword ds;                  // Data segment selector
    u_dword edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    u_dword int_no, err_code;    // Interrupt number and error code (if applicable)
    u_dword eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers;*/

#endif //MUSHOS_DESCRIPTOR_TABLES_H
