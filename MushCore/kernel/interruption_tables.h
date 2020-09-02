#ifndef MUSHOS_INTERRUPTION_TABLES_H
#define MUSHOS_INTERRUPTION_TABLES_H

#include "../../MushLib/generic.h"

typedef struct {
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

typedef struct {
    IDT_entry entries [256];
    IDT_descriptor descriptor;
} IDT;

void init_interruptions();

typedef struct {
    u_dword ds;                  // Data segment selector
    u_dword edi, esi, ebp, useless, ebx, edx, ecx, eax; // Pushed by pusha.
    u_dword int_no, err_code;    // Interrupt number and error code (if applicable)
    u_dword eip, cs, eflags, esp, ss; // Pushed by the processor automatically.
} registers;

typedef void (*interruption_handler)(registers* regs);
void set_interrupt_handler(u_byte n, interruption_handler handler);

#endif //MUSHOS_INTERRUPTION_TABLES_H
