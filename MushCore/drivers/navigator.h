#ifndef MUSHOS_NAVIGATOR_H
#define MUSHOS_NAVIGATOR_H

#include "../../MushLib/generic.h"


/**
 * Orbital slot map:
 *  ┌─────────┬─────────┬───────────────┐
 *  │  Heap   │  Stack  │  Code & Data  │
 *  ├─────────┼─────────┼───────────────┤
 *  │         │         │               │
 *  │ 0x3000b │ 0x1000b │    0x4000b    │
 *  │         │         │               │
 *  └─────────┴─────────┴───────────────┘
 * 0x0     0x3000    0x4000          0x8000
 */


#define move_stack_to(new_stack) {\
    asm volatile ("mov %%esp, (%0)" :: "r"(new_stack - 4));\
    asm volatile ("mov %%ebp, (%0)" :: "r"(new_stack - 8));\
    asm volatile ("mov %0, %%esp" :: "r"(new_stack - 8));\
    asm volatile ("mov %0, %%ebp" :: "r"(new_stack - 8));\
}

#define move_stack_back() {\
    asm volatile ("pop %ebp");\
    asm volatile ("pop %esp");\
};


typedef struct {
    dword elf;

    byte bits;
    byte endianness;
    byte current;
    byte os;

    dword unused1;
    dword unused2;

    word type;
    word architecture;
    dword version;

    dword entry;
    dword program_header_offset;
    dword section_header_offset;

    dword flags;
    word header_size;

    word program_header_size;
    word program_header_number;
    word sections_size;
    word sections_number;

    word section_names_offset;
} elf_file_header;

typedef struct {
    dword type;
    dword segment_offset;

    dword virtual_address;
    dword physical_address;

    dword segment_file_size;
    dword segment_size;

    dword flags;
    dword alignment;
} elf_program_header;



void domestic_launch(string filename, u_dword slot);

#endif //MUSHOS_NAVIGATOR_H
