#include "pages.h"
#include "../mushlib/logger.h"
#include "../mushlib/memory.h"
#include "interruptions.h"

page_directory* kernel_directory;
extern u_dword placement_address;
extern byte sentinel;
u_dword memory_end = 0x1000000; // 16 MB as for now



static page_table* get_page_table_pointer(page_table_pointer table) {
    return (page_table*) (table.frame * 0x1000);
}

static page_table_pointer create_page_table_entry(const byte* pointer, u_dword tail) {
    page_table_pointer page_table;
    page_table.frame = (u_dword) pointer / 0x1000;
    page_table.size = ((tail & 0x00000080u) == 0) ? 0 : 1;
    page_table.accessed = ((tail & 0x00000020u) == 0) ? 0 : 1;
    page_table.cache = ((tail & 0x00000010u) == 0) ? 0 : 1;
    page_table.write = ((tail & 0x00000008u) == 0) ? 0 : 1;
    page_table.user = ((tail & 0x00000004u) == 0) ? 0 : 1;
    page_table.rw = ((tail & 0x00000002u) == 0) ? 0 : 1;
    page_table.present = ((tail & 0x00000001u) == 0) ? 0 : 1;
    return page_table;
}

static u_dword get_page_pointer(page_pointer* page) {
    return page->frame * 0x1000;
}

static page_pointer create_page_entry(const byte* pointer, u_dword tail) {
    page_pointer page_table;
    page_table.frame = (u_dword) pointer / 0x1000;
    page_table.global = ((tail & 0x00000100u) == 0) ? 0 : 1;
    page_table.dirty = ((tail & 0x00000040u) == 0) ? 0 : 1;
    page_table.accessed = ((tail & 0x00000020u) == 0) ? 0 : 1;
    page_table.cache = ((tail & 0x00000010u) == 0) ? 0 : 1;
    page_table.write = ((tail & 0x00000008u) == 0) ? 0 : 1;
    page_table.user = ((tail & 0x00000004u) == 0) ? 0 : 1;
    page_table.rw = ((tail & 0x00000002u) == 0) ? 0 : 1;
    page_table.present = ((tail & 0x00000001u) == 0) ? 0 : 1;
    return page_table;
}

page_pointer* get_page(u_dword address, u_dword make_tail, page_directory *dir) {
    if (address > memory_end) return nullptr;
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    u_dword table_idx = address / 1024;
    if (get_page_table_pointer(dir->tablesPhysical[table_idx])) { // If this table is already assigned
        return &get_page_table_pointer(dir->tablesPhysical[table_idx])->pages[address % 1024];
    } else if (make_tail) {
        byte* page_table_pointer = (byte*) k_malloc_aligned(sizeof(page_table_pointer), true);
        dir->tablesPhysical[table_idx] = create_page_table_entry(page_table_pointer, make_tail);
        memory_clear(page_table_pointer, sizeof(page_table), 0);
        return &get_page_table_pointer(dir->tablesPhysical[table_idx])->pages[address % 1024];
    } else return nullptr;
}



void* get_page_address(u_dword address) {
    page_pointer* pointer = get_page(address, 0, kernel_directory);
    if (pointer) return (u_dword*) get_page_pointer(pointer);
    else return nullptr;
}



void initialise_paging() {
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    placement_address = (u_dword) &sentinel;

    // Let's make a page directory.
    kernel_directory = (page_directory*) k_malloc_aligned(sizeof(page_directory), true);
    memory_clear((byte*) kernel_directory, sizeof(page_directory), 0);

    for (u_dword j = 0; j < 0x100000; j += 0x1000)
        *get_page(j, 0x00000003, kernel_directory) = create_page_entry((const byte*) j, 0x00000003);

    // Before we enable paging, we must register our page fault handler.
    set_interrupt_handler(14, page_fault);

    // Now, enable paging!
    asm volatile("mov %0, %%cr3":: "r"(&kernel_directory->tablesPhysical));
    u_dword cr0, cr3;
    asm volatile("mov %%cr3, %0": "=r"(cr3));
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000001; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
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
