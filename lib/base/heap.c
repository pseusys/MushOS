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


/**
 * Internal function for determining whether the structure is inside the heap or outside.
 * @return true is structure is in heap, false otherwise.
 */
static boolean is_in_heap(void* structure) {
    return structure > header->heap_start && structure < header->heap_end;
}

/**
 * Internal function for getting heap header of any structure located in heap.
 * It just substracts heap block header size from pointer and casts it to heap block header.
 * Throws heap exception if the given pointer is outside of the current heap. TODO: custom command for throw.
 * @param structure a pointer to the structure.
 * @return heap_block_header pointer.
 */
static heap_block_header* get_header(void* structure) {
    if (is_in_heap(structure)) return (heap_block_header*) (structure - sizeof(heap_block_header));
    else throw_verbose(heap_exception_id, heap_exception_type, "Requested structure is located outside of the heap!")
}

/**
 * Function for structure located in heap size calculation (in bytes).
 * Throws heap exception if the given pointer is outside of the current heap.
 * @param structure a pointer to the structure.
 * @return structure size (in bytes).
 */
u_dword size(void* structure) {
    return get_header(structure)->size;
}

/**
 * Function for heap occupation calculation.
 * @return percent of allocated space in this heap.
 */
precise occupation() {
    if (!header->first_address) return 0;
    u_dword sum = 0;

    void* current_address = header->first_address;
    while (current_address) {
        heap_block_header* current_block = get_header(current_address);
        sum += current_block->size;
        current_address = current_block->next;
    }

    return (precise) (header->heap_end - header->heap_start) / sum;
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
void handle_allocation_exception() {
    terminate(heap_exception_id);
}


/**
 * Function for heap initialization, sets up singleton heap header.
 * This implementation of heap has fixed size, and so doesn't rely on paging.
 * That's enough for kernel and basic app needs, TODO: provide another heap implementation for extended MushLib.
 * @param start_address pointer to the start of the heap.
 * @param size size of the heap.
 */
void initialize_heap(void* start_address, u_dword size) {
    header = start_address;
    memory_clear(start_address, sizeof(heap_header), 0);
    header->heap_start = start_address + sizeof(heap_header);
    header->heap_end = start_address + size;
    header->first_address = nullptr;
    handle_exceptions(allocation_exception_id, handle_allocation_exception);
}

/**
 * Function for allocation of a heap block.
 * Searches for a place of suitable size to fit a structure of requested size in.
 * Throws allocation exception if no free space is available.
 * @param size requested block size.
 * @return pointer to the neewly allocated block.
 */
void* malloc(u_dword size) {
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

/**
 * Function for heap block size alteration.
 * Attempts to alter size in-place and copys the block only if in-place growth is not available.
 * Throws heap exception if the structure is located outside of the heap.
 * @param structure pointer to the structure for reallocation.
 * @param new_size new requested size of the structure.
 * @return pointer to the new structure.
 */
void* realloc(void* structure, u_dword new_size) {
    heap_block_header* block = get_header(structure);
    if (new_size < block->size) {
        block->size = new_size;
    } else if (new_size > block->size) {
        if (new_size < block->next - structure) block->size = new_size;
        else {
            void* new_structure = malloc(new_size);
            memory_copy(structure, new_structure, block->size);
            free(structure);
            structure = new_structure;
        }
    }
    return structure;
}

/**
 * Function for heap block freeing.
 * Throws heap exception if the structure is located outside of the heap.
 * @param structure pointer to the structure for freeing.
 */
void free(void* structure) {
    heap_block_header* block = get_header(structure);
    if (structure == header->first_address) header->first_address = block->next;
    if (block->next) get_header(block->next)->previous = block->previous;
    if (block->previous) get_header(block->previous)->next = block->next;
}
