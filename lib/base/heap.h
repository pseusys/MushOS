/**
 * @file
 * 
 * @brief This is a standard MushLib heap header. See standard implementation in @ref lib/base/heap.c
 * 
 * This heap solves a strange C stdlib issue - each pointer in heap has a @link heap_block_header header @endlink
 *    that contains allocated data size.
 * One can determine allocated space (in pieces, e.g. array length) using @ref size_of macro.
 * 
 * It's also possible to determine whether a structure belongs to heap or not (using @ref is_in_heap function).
 */

#ifndef MUSHLIB_HEAP_H
#define MUSHLIB_HEAP_H

#include "generic.h"


/**
 * Allocation exception ID.
 * Indicates that a program asked for more space than heap can provide.
 * Will be thrown by default allocation exception handler.
 * Should be thrown if a custom program is terminated because of allocation exception.
 */
#define allocation_exception_id 0x1
/**
 * Allocation exception type.
 * May be used by custom allocation exception handlers.
 */
#define allocation_exception_type "Allocation Exception"

/**
 * Heap exception ID.
 * Indicates that there was an attempt to calculate size or free a structure that is located outside of the heap.
 * Will be thrown by default heap exception handler.
 * Should be thrown if a custom program is terminated because of heap exception.
 */
#define heap_exception_id 0x2
/**
 * Heap exception type.
 * May be used by custom heap exception handlers.
 */
#define heap_exception_type "Heap Exception"


/**
 * Macro for for structure located in heap size calculation (in units).
 * For example, can be used to calculate array of structures length.
 * Throws heap exception if the given pointer is outside of the current heap.
 * @param structure a pointer to the structure.
 * @return structure size (in units).
 */
#define size_of(structure) (size(structure) / sizeof(typeof(*structure)))

/**
 * Function for heap occupation calculation.
 * @return percent of allocated space in this heap.
 */
precise occupation();

/**
 * Function for heap initialization, sets up singleton heap header.
 * This implementation of heap has fixed size, and so doesn't rely on paging.
 * That's enough for kernel and basic app needs, TODO: provide another heap implementation for extended MushLib.
 * @param start_address pointer to the start of the heap.
 * @param initial_size initial size of the heap.
 */
void initialize_heap(void* start_address, u_dword initial_size);

/**
 * Function for allocation of a raw heap block.
 * Searches for a place of suitable size to fit a structure of requested size in.
 * Throws allocation exception if no free space is available.
 * @param size requested block size.
 * @return pointer to the neewly allocated block.
 */
void* ralloc(u_dword size);

/**
 * Function for allocation of a heap block and clearing it.
 * Does the same as `ralloc`, but sets every allocated byte to zero.
 * Throws allocation exception if no free space is available.
 * @param size requested block size.
 * @return pointer to the neewly allocated block.
 */
void* zalloc(u_dword size);

/**
 * Function for heap block size alteration.
 * Attempts to alter size in-place and copys the block only if in-place growth is not available.
 * Throws heap exception if the structure is located outside of the heap.
 * @param structure pointer to the structure for allocation size changing.
 * @param new_size new requested size of the structure.
 * @return pointer to the new structure.
 */
void* challoc(void* structure, u_dword new_size);

/**
 * Function for heap block unallocation.
 * Throws heap exception if the structure is located outside of the heap.
 * @param structure pointer to the structure for unallocation.
 */
void unalloc(void* structure);

/**
 * Function for determining whether the structure is inside the heap or outside.
 * @param structure a pointer to the structure.
 * @return true is structure is in heap, false otherwise.
 */
boolean is_in_heap(void* structure);

/**
 * Function for structure located in heap size calculation (in bytes).
 * Throws heap exception if the given pointer is outside of the current heap.
 * @param structure a pointer to the structure.
 * @return structure size (in bytes).
 */
u_dword size(void* structure);

#endif // MUSHLIB_HEAP_H
