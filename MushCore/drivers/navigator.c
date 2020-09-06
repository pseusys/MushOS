#include "navigator.h"
#include "../../MushLib/heap.h"
#include "../../MushCreator/file_system/structures.h"
#include "../../MushCreator/file_system/file_input.h"
#include "../kernel/pages.h"
#include "../../MushLib/stdio.h"


extern u_dword memory_start;
extern u_dword memory_end;

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

void domestic_launch(string filename) {
    file* executable = open_file_global(filename);
    info("Launching %s\n", get_name(executable))
    elf_file_header* header = (elf_file_header*) malloc(sizeof(elf_file_header));
    read_bytes(executable, (byte*) header, sizeof(elf_file_header), 0);
    info("Executable contains %d segments (at %h)\n", header->program_header_number, header->program_header_offset)

    elf_program_header* program_headers = (elf_program_header*) malloc(sizeof(elf_program_header) * header->program_header_number);
    seek_to(executable, header->program_header_offset);
    read_bytes(executable, (byte*) program_headers, size(program_headers), 0);
    for (int i = 0; i < header->program_header_number; ++i) {
        if (program_headers[i].type == 1) {
            print_seg(&(program_headers[i]));
            if ((program_headers[i].virtual_address >= memory_start) &&
                (program_headers[i].virtual_address + program_headers[i].segment_size <= memory_end)) {
                u_dword memory_offset = 0, segment_offset = program_headers[i].segment_offset;
                while (memory_offset + page_size < program_headers[i].segment_size) {
                    void *page = get_page_address(program_headers[i].virtual_address + memory_offset);
                    info("Accessing page %p", page)
                    seek_to(executable, segment_offset);
                    read_bytes(executable, page, page_size, 0);
                    memory_offset += page_size;
                    segment_offset += page_size;
                }
                u_dword remains = program_headers[i].segment_size - memory_offset;
                info("Accessing page %h\n", program_headers[i].virtual_address + memory_offset)
                void *page = get_page_address(program_headers[i].virtual_address + memory_offset);
                info("Transferring %h bytes from offset %h in file to pos %h on memory...\n", remains, segment_offset, page)
                seek_to(executable, segment_offset);
                read_bytes(executable, page, remains, 0);
            } else {
                bad("Fatal error: segment in orbital app %s is wrongly positioned.\n", get_name(executable))
                bad("%h <= %h\n", program_headers[i].virtual_address, memory_start)
                bad("%h >= %h\n", program_headers[i].virtual_address + program_headers[i].segment_size, memory_end)
                return;
            }
        } else info("Skipping non-loadable segment (type %h)...\n", program_headers[i].type)
    }

    info("Calling %p\n", header->entry)
    asm volatile ("call *%0" :: "r"(header->entry));
}
