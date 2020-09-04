#ifndef MUSHOS_NAVIGATOR_H
#define MUSHOS_NAVIGATOR_H

#include "../../MushLib/generic.h"


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



void domestic_launch(string filename);

#endif //MUSHOS_NAVIGATOR_H
