#ifndef MUSHOS_TASK_H
#define MUSHOS_TASK_H

#include "../../lib/base/generic.h"

#include "pages.h"


// This structure defines a 'task' - a process.
typedef struct task_struct {
   int id;                // Process ID.
   u_dword esp, ebp;       // Stack and base pointers.
   u_dword eip;            // Instruction pointer.
   page_folder *page_directory; // Page directory.
   struct task_struct *next;     // The next task in a linked list.
} task;

// Initialises the tasking system.
void initialise_tasking();

// Called by the timer hook, this changes the running process.
void switch_task();

// Forks the current process, spawning a new one with a different
// memory space.
u_dword fork();

// Returns the pid of the current process.
int getpid();

#endif //MUSHOS_TASK_H
