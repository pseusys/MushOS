#include "pages.h"
#include "../mushlib/logger.h"
#include "../mushlib/memory.h"
#include "interruptions.h"

page_directory_t* kernel_directory;
page_table_t* first;
extern u_dword placement_address;



static u_dword get_page_pointer(page_t* page_p) {
    return page_p->frame * 0x1000;
}

u_dword* get_page_address(u_dword address) {
    return (u_dword*) get_page_pointer(get_page(address, false, kernel_directory));
}



extern byte sentinel;

void initialise_paging()
{
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    placement_address = (u_dword) &sentinel;
    im("Placement address is: ")in(placement_address, HEXADECIMAL, POINTER)endl()
    u_dword mem_end_page = 0x10000;

    /*nframes = mem_end_page / 0x1000;
    frames = (u_dword*) k_malloc(INDEX_FROM_BIT(nframes));
    memory_clear((byte*) frames, INDEX_FROM_BIT(nframes), 0);

    // Let's make a page directory.
    kernel_directory = (page_directory_t*) k_malloc_aligned(sizeof(page_directory_t), true);
    memory_clear((byte*) kernel_directory, sizeof(page_directory_t), 0);
    current_directory = kernel_directory;

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    int i = 0;
    while (i < placement_address)
    {
        // Kernel code is readable but not writeable from userspace.
        page_t* p = get_page(i, 1, kernel_directory);
        alloc_frame(p, 0, 0);
        gn(p, HEXADECIMAL, POINTER)
        bb()bn(p->user, DECIMAL, BOOLEAN)bm(" ")bn(p->present, DECIMAL, BOOLEAN)endl()
        i += 0x1000;
    }*/

    kernel_directory = (page_directory_t*) k_malloc_aligned(sizeof(page_directory_t), true);
    memory_clear((byte*) kernel_directory, sizeof(page_directory_t), 0);
    first = (page_table_t*) k_malloc_aligned(sizeof(page_table_t), true);
    memory_clear((byte*) first, sizeof(page_table_t), 0);

    int counter = 0;
    for (u_dword j = 0; j < 0x100000; j += 0x1000) {
        page_t* page = get_page(j, true, kernel_directory);
        page->frame = j / 0x1000;
        page->user = false;
        page->present = true;
        page->rw = true;
        counter++;
    }

    // Before we enable paging, we must register our page fault handler.
    set_interrupt_handler(14, page_fault);

    for (int i = 0; i < 3; ++i) {
        print_number((dword) get_page(i * 0x1000, true, kernel_directory), POINTER, HEXADECIMAL, HIGH_MAGENTA, BLACK);
        print_color(" - ", HIGH_MAGENTA, BLACK);
        print_number((dword) &kernel_directory->tablesPhysical[0]->pages[i], POINTER, HEXADECIMAL, HIGH_MAGENTA, BLACK);
        endl()
    }

    im("Placement address is: ")in(placement_address, HEXADECIMAL, POINTER)endl()
    im("Kernel dir address is: ")in((u_dword) kernel_directory, HEXADECIMAL, POINTER)endl()
    im("First dir address is: ")in((u_dword) first, HEXADECIMAL, POINTER)endl()
    im("Page fault address is: ")in((u_dword) page_fault, HEXADECIMAL, POINTER)endl()

    // Now, enable paging!
    switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir) {
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    u_dword cr0, cr3;
    asm volatile("mov %%cr3, %0": "=r"(cr3));
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000001; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}



page_t* get_page(u_dword address, int make, page_directory_t *dir) {
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    u_dword table_idx = address / 1024;
    if (dir->tablesPhysical[table_idx]) { // If this table is already assigned
        return &((page_table_t*) ((u_dword) dir->tablesPhysical[table_idx] & 0xffff000u))->pages[address % 1024];
    } else if (make) {
        page_t* page_pointer;
        dir->tablesPhysical[table_idx] = (page_table_t*) k_malloc_aligned(sizeof(page_table_t), true);
        memory_clear((byte*) dir->tablesPhysical[table_idx], sizeof(page_table_t), 0);
        page_pointer = &dir->tablesPhysical[table_idx]->pages[address % 1024];
        dir->tablesPhysical[table_idx] = (page_table_t*) (((u_dword) dir->tablesPhysical[table_idx] & 0xffff000u) | 0x0000003u); // PRESENT, RW, US.
        return page_pointer;
    } else {
        return nullptr;
    }
}



void page_fault(registers* regs)
{
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    u_dword faulting_address;
    asm ("mov %%cr2, %0" : "=r" (faulting_address));

    // The error code gives us details of what happened.
    int present   = !(regs->err_code & 0x1); // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

    // Output an error message.
    bm("Page fault! ( ")
    if (present) {bm("present ")}
    if (!rw) {bm("read-only ")}
    if (us) {bm("user-mode ")}
    if (reserved) {bm("reserved ")}
    bm(") at ")bn(faulting_address, HEXADECIMAL, POINTER)endl()
    asm("jmp .");
}
