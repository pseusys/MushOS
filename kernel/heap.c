#include "heap.h"
#include "../mushlib/memory.h"
#include "../mushlib/logger.h"

heap_header* header;



static block_header* get_header(void* structure) {
    return (block_header*) (structure - sizeof(block_header));
}

u_dword size(void* structure) {
    return get_header(structure)->size;
}



void* allocate_space(void* free_pointer, u_dword size, block_header* previous, block_header* next) {
    ((block_header*) free_pointer)->size = size;
    ((block_header*) free_pointer)->previous = previous;
    ((block_header*) free_pointer)->next = next;

    void* memory_address = free_pointer + sizeof(block_header);
    if (previous) get_header(previous)->next = memory_address;
    if (next) get_header(next)->previous = memory_address;
    return memory_address;
}



void initialize_heap(void* start_address, u_dword size) {
    header = start_address;
    memory_clear(start_address, sizeof(heap_header), 0);
    header->heap_start = start_address + sizeof(heap_header);
    header->heap_end = start_address + size - sizeof(heap_header);
    header->first_address = nullptr;
}

void* malloc(u_dword size) {
    if (header->first_address) {
        void* current_address = header->heap_start;
        void* next_address = header->first_address;

        u_dword free_space_size = next_address - current_address - sizeof(block_header);
        if (free_space_size >= (size + sizeof(block_header)))
            return allocate_space(current_address, size, nullptr, next_address);

        current_address = next_address;
        block_header* current_block = get_header(current_address);
        next_address = current_block->next;

        while (next_address) {
            free_space_size = next_address - current_address - current_block->size - sizeof(block_header);
            if (free_space_size >= (size + sizeof(block_header)))
                return allocate_space(current_address + current_block->size, size, current_address, next_address);

            current_address = next_address;
            current_block = get_header(current_address);
            next_address = current_block->next;
        }

        free_space_size = header->heap_end - current_address - current_block->size - sizeof(block_header);
        if (free_space_size >= (size + sizeof(block_header)))
            return allocate_space(current_address + current_block->size, size, current_address, nullptr);

        return nullptr;

    } else {
        header->first_address = allocate_space(header->heap_start, size, nullptr, nullptr);
        return header->first_address;
    }
}

void free(void* structure) {
    print_number((u_dword) structure, INTEGER, HEXADECIMAL, WHITE, BLACK);
    block_header* block = get_header(structure);
    if (structure == header->first_address) header->first_address = block->next;
    if (block->next) block->next->previous = block->previous;
    if (block->previous) block->previous->next = block->next;
}
