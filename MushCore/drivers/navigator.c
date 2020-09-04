#include "navigator.h"
#include "../../MushLib/heap.h"
#include "../../MushCreator/file_system/structures.h"
#include "../../MushCreator/file_system/file_input.h"
#include "../kernel/pages.h"
#include "../../MushLib/stdio.h"


extern u_dword memory_start;
extern u_dword memory_end;

void domestic_launch(string filename) {
    file* executable = open_file_global(filename);
    info("Launching %s\n", get_name(executable))
    info("File opened to %p\n", executable)
    elf_file_header* header = (elf_file_header*) malloc(sizeof(elf_file_header));
    read_bytes(executable, (byte*) header, sizeof(elf_file_header), 0);
    info("Executable contains %d segments\n", header->program_header_number)

    elf_program_header* program_headers = (elf_program_header*) malloc(sizeof(elf_program_header) * header->program_header_number);
    read_bytes(executable, (byte*) program_headers, sizeof(elf_program_header), header->program_header_offset);
    for (int i = 0; i < header->program_header_number; ++i) {
        //if (program_headers[i].type == 1) {
            if ((program_headers[i].virtual_address < memory_start) &&
                (program_headers[i].virtual_address + program_headers[i].segment_size > memory_end)) {
                u_dword memory_offset = 0, segment_offset = program_headers[i].segment_offset;
                while (memory_offset + page_size < program_headers[i].segment_size) {
                    void *page = get_page_address(program_headers[i].virtual_address + memory_offset);
                    info("Accessing page %p", page)
                    read_bytes(executable, page, page_size, segment_offset);
                    memory_offset += page_size;
                    segment_offset += page_size;
                }
                u_dword remains = program_headers[i].segment_size - memory_offset;
                void *page = get_page_address(program_headers[i].virtual_address + memory_offset);
                info("Accessing page %p\n", page)
                read_bytes(executable, page, remains, segment_offset);
            } else {
                bad("Segment %d ")
                bad("Fatal error: segment in orbital app %s is wrongly positioned.\n", get_name(executable))
                bad("%d < %d\n", program_headers[i].virtual_address, memory_start)
                bad("%d > %d\n", program_headers[i].virtual_address + program_headers[i].segment_size, memory_end)
                return;
            }
        //} else info("Skipping non-loadable segment...\n")
    }

    info("Calling %p\n", header->entry)
    asm volatile ("call *%0" :: "r"(header->entry));
}
