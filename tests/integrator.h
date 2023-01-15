#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#define heap_size 8000

void* allocate_space_for_heap(int size);

void check_heap_for_leaks();

#endif // INTEGRATOR_H
