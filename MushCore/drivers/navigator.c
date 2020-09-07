#include "navigator.h"
#include "../../MushLib/heap.h"
#include "../../MushCreator/file_system/structures.h"
#include "../../MushCreator/file_system/file_input.h"
#include "../kernel/pages.h"
#include "../../MushLib/stdio.h"


extern u_dword memory_start;
extern u_dword orbit_start;
static u_dword entry_point;

void print_seg(elf_program_header* header) {
    info("Segment info:\n")
    info("\tSegment type: %h\n", header->type)
    info("\tSegment offset: %h\n\n", header->segment_offset)
    info("\tSegment virtual address: %h\n", header->virtual_address)
    info("\tSegment physical address: %h\n\n", header->physical_address)
    info("\tSegment size in file: %h\n", header->segment_file_size)
    info("\tSegment size in memory: %h\n\n", header->segment_size)
    info("\tSegment flags: %h\n", header->flags)
    info("\tSegment alignment: %h\n\n", header->alignment)
}

void domestic_launch(string filename, u_dword slot) {
    file* executable = open_file_global(filename);
    info("Launching %s\n", get_name(executable))
    elf_file_header* header = (elf_file_header*) malloc(sizeof(elf_file_header));
    read_bytes(executable, (byte*) header, sizeof(elf_file_header), 0);
    info("Executable contains %d segments (at %h)\n", header->program_header_number, header->program_header_offset)

    elf_program_header* program_headers = (elf_program_header*) malloc(sizeof(elf_program_header) * header->program_header_number);
    seek_to(executable, header->program_header_offset);
    read_bytes(executable, (byte*) program_headers, size(program_headers), 0);

    u_dword slot_number = orbit_start + (slot * 0x1000);

    for (int i = 0; i < header->program_header_number; ++i) {
        if (program_headers[i].type == 1) {
            //print_seg(&(program_headers[i]));
            u_dword slot_address = program_headers[i].virtual_address;
            u_dword data_size = program_headers[i].segment_size;
            u_dword data_offset = program_headers[i].segment_offset;
            if ((slot_number >= orbit_start) && (data_size <= page_size * 4) && (slot_address + data_size < memory_start)) {
                for (int j = slot_number; j < slot_number + 0x8000; j += page_size) get_page_address(j);
                info("Transferring %h bytes from offset %h in file to pos %h on memory...\n", data_size, data_offset, slot_address)
                seek_to(executable, data_offset);
                read_bytes(executable, (void*) slot_address, page_size, 0);
            } else {
                bad("Fatal error: segment in orbital app %s is wrongly positioned.\n", get_name(executable))
                bad("%h <= %h\n", program_headers[i].virtual_address, orbit_start)
                bad("%h >= %h\n", program_headers[i].virtual_address + program_headers[i].segment_size, memory_start)
                return;
            }
        } else info("Skipping non-loadable segment (type %h)...\n", program_headers[i].type)
    }

    entry_point = header->entry;
    info("Calling slot %p, point %p\n", slot_number, entry_point)
    move_stack_to(slot_number + 0x4000)
    asm volatile ("call *%0" :: "r"(entry_point));
    move_stack_back()
}
