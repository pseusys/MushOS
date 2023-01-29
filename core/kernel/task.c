#include "task.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/heap.h"
#include "../../lib/base/memory.h"
#include "../../lib/base/stdio.h"

#include "pages.h"


extern page_folder* kernel_directory, * current_directory;
extern page_folder* clone_directory(page_folder* src);

extern u_dword get_instruction_pointer();

// The currently running task.
// The start of the task linked list.
volatile task* current_task, * ready_queue;

// The next available process ID.
u_dword next_pid = 0;


void initialise_tasking() {
   asm volatile("cli");

   // Initialise the first task (kernel task)
   current_task = ready_queue = (task*) ralloc(sizeof(task));
   current_task->id = next_pid++;
   current_task->esp = current_task->ebp = 0;
   current_task->eip = 0;
   current_task->page_directory = current_directory;
   current_task->next = 0;

   // Reenable interrupts.
   asm volatile("sti");
}


u_dword fork() {
    // We are modifying kernel structures, and so cannot be interrupted.
    asm volatile("cli");

    // Take a pointer to this process' task struct for later reference.
    task *parent_task = (task*) current_task;

    // Clone the address space.
    page_folder* directory = clone_directory(current_directory);

    // Create a new process.
    task* new_task = (task*) ralloc(sizeof(task));
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->next = 0;

    // Add it to the end of the ready queue.
    // Find the end of the ready queue...
    task* tmp_task = (task*) ready_queue;
    while (tmp_task->next) tmp_task = tmp_task->next;
    // ...And extend it.
    tmp_task->next = new_task;

    // This will be the entry point for the new process.
    u_dword eip = get_instruction_pointer();

    // We could be the parent or the child here - check.
    if (current_task == parent_task) {
        // We are the parent, so set up the esp/ebp/eip for our child.
        asm volatile("mov %%esp, %0" : "=r"(new_task->esp));
        asm volatile("mov %%ebp, %0" : "=r"(new_task->ebp));
        new_task->eip = eip;
        // All finished: Reenable interrupts.
        asm volatile("sti");
        return new_task->id;
    } else {
        return 0; // We are the child - by convention return 0.
    }
}

void switch_task() {
    // If we haven't initialised tasking yet, just return.
    if (!current_task) return;

    // Read esp, ebp now for saving later on.
    u_dword esp, ebp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    
    // Read the instruction pointer. We do some cunning logic here:
    // One of two things could have happened when this function exits -
    // (a) We called the function and it returned the EIP as requested.
    // (b) We have just switched tasks, and because the saved EIP is essentially
    // the instruction after read_eip(), it will seem as if read_eip has just
    // returned.
    // In the second case we need to return immediately. To detect it we put a dummy
    // value in EAX further down at the end of this function. As C returns values in EAX,
    // it will look like the return value is this dummy value! (0xDEADBABA).
    u_dword eip = get_instruction_pointer();

    // Have we just switched tasks?
    if (eip == 0xDEADBABA) return;

    // No, we didn't switch tasks. Let's save some register values and switch.
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;

    // Get the next task to run.
    current_task = current_task->next;
    // If we fell off the end of the linked list start again at the beginning.
    if (current_task == nullptr) current_task = ready_queue;
    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    // Make sure the memory manager knows we've changed page directory.
    current_directory = current_task->page_directory;

    // Here we:
    // * Stop interrupts so we don't get interrupted.
    // * Temporarily put the new EIP location in ECX.
    // * Load the stack and base pointers from the new task struct.
    // * Change page directory to the physical address (physicalAddr) of the new directory.
    // * Put a dummy value (0xDEADBABA) in EAX so that above we can recognise that we've just
    // switched task.
    // * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
    // the next instruction.
    // * Jump to the location in ECX (remember we put the new EIP in there).
    asm volatile(
        "cli;\n"
        "mov %1, %%esp;\n"
        "mov %2, %%ebp;\n"
        "mov %3, %%cr3;\n"
        "mov $0xDEADBABA, %%eax;\n"
        "sti;\n"
        "jmp *%%ecx;\n"
        :: "c"(eip), "rm"(esp), "rm"(ebp), "r"(&current_directory->contents)
    );
}


int getpid() {
    return current_task->id;
}
