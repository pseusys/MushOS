/**
 * @file
 * 
 * @brief This is a standard implementation of heap for MushLib.
 * 
 * The heap doesn't use paging, so it has a limited size.
 * This allows kernel to use it right after launch, but there's also a significant drawback:
 *    when kernel heap gets overflown, no standard exception can be shown (because it requires heap space allocation).
 * The drawback is handled with @link core/kernel/heap.c kernel allocation exception handler @endlink.
 * Any non-kernel apps may set up their own kernel exception handler (using @ref allocation_exception_id)
 *    or just do nothing - default allocation exception handler will allow kernel to terminate safely any app with overflown heap.
 */

#include "heap.h"

#include "memory.h"
#include "exceptions.h"


/**
 * Header of the heap.
 * Defines current heap: its start and end addresses and address of the first allocated block.
 * Located in the very beginning of the heap.
 */
typedef struct {
    void* first_address;
    void* heap_start, * heap_end;
} heap_header;

/**
 * Header of an allocated heap block.
 * Defines current block, contain its size.
 * Also acts as a linked list, contains addresses of the previous and next allocated blocks.
 */
typedef struct {
    u_dword size;
    void* previous, * next;
} heap_block_header;


/**
 * Header of the current app heap.
 * Every app in MushOS has its own heap.
 * This implementation of heap is a singleton.
 */
heap_header* header;


boolean is_in_heap(void* structure) {
    return structure > header->heap_start && structure < header->heap_end;
}

/**
 * Internal function for getting heap header of any structure located in heap.
 * It just substracts heap block header size from pointer and casts it to heap block header.
 * Throws heap exception if the given pointer is outside of the current heap.
 * @param structure a pointer to the structure.
 * @return heap_block_header pointer.
 */
static heap_block_header* get_header(void* structure) {
    if (is_in_heap(structure)) return (heap_block_header*) (structure - sizeof(heap_block_header));
    else throw_verbose(heap_exception_id, heap_exception_type, "Requested structure is located outside of the heap!")
}

u_dword size(void* structure) {
    return get_header(structure)->size;
}

precise occupation() {
    if (!header->first_address) return 0.0;
    u_dword sum = 0;
    u_dword num = 0;

    void* current_address = header->first_address;
    while (current_address) {
        heap_block_header* current_block = get_header(current_address);
        sum += current_block->size;
        num++;
        current_address = current_block->next;
    }

    return (precise) num;
}


/**
 * Internal function for space allocation in heap.
 * Sets up and fills heap block header, optionally alters headers of the next and previous heap blocks.
 * @param free_pointer pointer to free space (heap block header size + required size).
 * @param size required size of heap block.
 * @param previous previous heap block.
 * @param next next heap block.
 * @return pointer to the start of the block.
 */
static void* allocate_space(void* free_pointer, u_dword size, heap_block_header* previous, heap_block_header* next) {
    ((heap_block_header*) free_pointer)->size = size;
    ((heap_block_header*) free_pointer)->previous = previous;
    ((heap_block_header*) free_pointer)->next = next;

    void* memory_address = free_pointer + sizeof(heap_block_header);
    if (previous) get_header(previous)->next = memory_address;
    if (next) get_header(next)->previous = memory_address;
    return memory_address;
}


/**
 * Allocation exception can't be handled regularily: handling requires heap.
 * So, heap exception terminate app without any on-screen explanation.
 */
static void handle_allocation_exception() {
    terminate(heap_exception_id);
}


void initialize_heap(void* start_address, u_dword initial_size) {
    header = start_address;
    memory_clear(start_address, sizeof(heap_header), 0);
    header->heap_start = start_address + sizeof(heap_header);
    header->heap_end = start_address + initial_size;
    header->first_address = nullptr;
    handle_exceptions(allocation_exception_id, handle_allocation_exception);
}

void* ralloc(u_dword size) {
    if (!header->first_address) {
        header->first_address = allocate_space(header->heap_start, size, nullptr, nullptr);
        return header->first_address;
    }
    
    void* best_address = nullptr, * best_previous = nullptr, * best_next = nullptr;
    u_dword best_size = header->heap_end - header->heap_start;

    void* current_address = header->heap_start;
    void* next_address = header->first_address;

    u_dword free_space_size = next_address - current_address - sizeof(heap_block_header);
    if ((free_space_size >= (size + sizeof(heap_block_header))) && (free_space_size < best_size)) {
        best_size = free_space_size;
        best_address = current_address; best_previous = nullptr; best_next = next_address;
    }

    current_address = next_address;
    heap_block_header* current_block = get_header(current_address);
    next_address = current_block->next;

    while (next_address) {
        free_space_size = next_address - current_address - current_block->size - sizeof(heap_block_header);
        if ((free_space_size >= (size + sizeof(heap_block_header))) && (free_space_size < best_size)) {
            best_size = free_space_size;
            best_address = current_address + current_block->size; best_previous = current_address; best_next = next_address;
        }

        current_address = next_address;
        current_block = get_header(current_address);
        next_address = current_block->next;
    }

    free_space_size = header->heap_end - current_address - current_block->size - sizeof(heap_block_header);
    if ((free_space_size >= (size + sizeof(heap_block_header))) && (free_space_size < best_size)) {
        best_address = current_address + current_block->size; best_previous = current_address; best_next = nullptr;
    }

    void* final_address = nullptr;
    if (best_address != nullptr) final_address = allocate_space(best_address, size, best_previous, best_next);
    if (best_address == header->heap_start) header->first_address = final_address;
    if (final_address == nullptr) throw_verbose(allocation_exception_id, allocation_exception_type, "No space left in heap available for allocation!")
    return final_address;
}

void* zalloc(u_dword size) {
    void* result = ralloc(size);
    memory_clear((byte*) result, size, 0);
    return result;
}

void* challoc(void* structure, u_dword new_size) {
    heap_block_header* block = get_header(structure);
    if (new_size < block->size) {
        block->size = new_size;
    } else if (new_size > block->size) {
        if (new_size < block->next - structure) block->size = new_size;
        else {
            void* new_structure = ralloc(new_size);
            memory_copy(structure, new_structure, block->size);
            unalloc(structure);
            structure = new_structure;
        }
    }
    return structure;
}

void unalloc(void* structure) {
    heap_block_header* block = get_header(structure);
    if (structure == header->first_address) header->first_address = block->next;
    if (block->next) get_header(block->next)->previous = block->previous;
    if (block->previous) get_header(block->previous)->next = block->next;
}
