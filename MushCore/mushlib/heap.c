#include "heap.h"
#include "memory.h"

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
        void* best_address = nullptr, * best_previous = nullptr, * best_next = nullptr;
        u_dword best_size = header->heap_end - header->heap_start;

        void* current_address = header->heap_start;
        void* next_address = header->first_address;

        u_dword free_space_size = next_address - current_address - sizeof(block_header);
        if ((free_space_size >= (size + sizeof(block_header))) && (free_space_size < best_size)) {
            best_size = free_space_size;
            best_address = current_address; best_previous = nullptr; best_next = next_address;
        }

        current_address = next_address;
        block_header* current_block = get_header(current_address);
        next_address = current_block->next;

        while (next_address) {
            free_space_size = next_address - current_address - current_block->size - sizeof(block_header);
            if ((free_space_size >= (size + sizeof(block_header))) && (free_space_size < best_size)) {
                best_size = free_space_size;
                best_address = current_address + current_block->size; best_previous = current_address; best_next = next_address;
            }

            current_address = next_address;
            current_block = get_header(current_address);
            next_address = current_block->next;
        }

        free_space_size = header->heap_end - current_address - current_block->size - sizeof(block_header);
        if ((free_space_size >= (size + sizeof(block_header))) && (free_space_size < best_size)) {
            best_address = current_address + current_block->size; best_previous = current_address; best_next = nullptr;
        }

        if (best_address != nullptr) best_address = allocate_space(best_address, size, best_previous, best_next);
        return best_address;

    } else {
        header->first_address = allocate_space(header->heap_start, size, nullptr, nullptr);
        return header->first_address;
    }
}

void* realloc(void* structure, u_dword new_size) {
    block_header* block = get_header(structure);
    if (new_size < block->size) {
        block->size = new_size;
    } else if (new_size > block->size) {
        if (new_size < block->next - structure) block->size = new_size;
        else {
            free(structure);
            void* new_structure = malloc(new_size);
            memory_copy(structure, new_structure, block->size);
            structure = new_structure;
        }
    }
    return structure;
}


void free(void* structure) {
    block_header* block = get_header(structure);
    if (structure == header->first_address) header->first_address = block->next;
    if (block->next) get_header(block->next)->previous = block->previous;
    if (block->previous) get_header(block->previous)->next = block->next;
}