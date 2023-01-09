#include "pages.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/stdio.h"
#include "../../lib/base/memory.h"

#include "../drivers/screen.h"

#include "placement.h"


#define page_size 0x1000

page_directory* kernel_directory, * current_directory;
u_dword orbit_start = 0x100000;
u_dword memory_start = 0x200000;
u_dword memory_end = 0xf00000;
u_dword memory_edge = 0x1000000;

void* page_dirs = (void*) 0x60000;
void* page_tables = (void*) 0x20000;

u_byte* page_pool;
u_byte* page_tables_pool;
u_dword page_dirs_pool;


static page_table* get_page_table_pointer(page_table_pointer table) {
    return (page_table*) (table.frame * page_size);
}

static page_table_pointer create_page_table_entry(page_table* pointer, u_dword tail) {
    page_table_pointer page_table;
    page_table.frame = (u_dword) pointer / page_size;
    page_table.size = ((tail & 0x00000080u) == 0) ? 0 : 1;
    page_table.accessed = ((tail & 0x00000020u) == 0) ? 0 : 1;
    page_table.cache = ((tail & 0x00000010u) == 0) ? 0 : 1;
    page_table.write = ((tail & 0x00000008u) == 0) ? 0 : 1;
    page_table.user = ((tail & 0x00000004u) == 0) ? 0 : 1;
    page_table.rw = ((tail & 0x00000002u) == 0) ? 0 : 1;
    page_table.present = ((tail & 0x00000001u) == 0) ? 0 : 1;
    return page_table;
}

static void* get_page_pointer(page_pointer page) {
    return (void*) (page.frame * page_size);
}

static page_pointer create_page_entry(const byte* pointer, u_dword tail) {
    page_pointer page_table;
    page_table.frame = (u_dword) pointer / page_size;
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



static void* allocate_page() {
    u_dword num = (memory_end - orbit_start) / page_size;
    for (u_dword i = 0; i < num; ++i) {
        if (page_pool[i] != 0xff) {
            for (u_dword j = 0; j < 8; ++j) {
                if (!(page_pool[i] & (1u << j))) {
                    page_pool[i] |= (1u << j);
                    return (void *) (orbit_start + (i * 8 * page_size) + (j * page_size));
                }
            }
        }
    }
    return nullptr;
}

page_table* allocate_page_table() {
    u_dword number = (page_dirs - page_tables) / sizeof(page_table);
    for (u_dword i = 0; i < number; ++i) {
        if (page_tables_pool[i] != 0xff) {
            for (u_dword j = 0; j < 8; ++j) {
                if (!(page_tables_pool[i] & (1u << j))) {
                    page_tables_pool[i] |= (1u << j);
                    page_table* address = (page_table *) (page_tables + (i * 8 * sizeof(page_table)) + (j * sizeof(page_table)));
                    memory_clear((byte*) address, sizeof(page_table), 0);
                    return address;
                }
            }
        }
    }
    return nullptr;
}


static page_pointer* get_page(u_dword address, u_dword make_tail, page_directory *dir) {
    if (address > memory_end) return nullptr;
    // Turn the address into an index.
    address /= page_size;
    // Find the page table containing this address.
    u_dword table_idx = address / 1024;
    if (get_page_table_pointer(dir->tablesPhysical[table_idx])) { // If this table is already assigned
        return &get_page_table_pointer(dir->tablesPhysical[table_idx])->pages[address % 1024];
    } else if (make_tail) {
        page_table* page_table_pointer = nullptr;
        if ((address < orbit_start) || (address >= memory_end))
            page_table_pointer = (page_table*) k_malloc_aligned(sizeof(page_table), true);
        else page_table_pointer = allocate_page_table();
        dir->tablesPhysical[table_idx] = create_page_table_entry(page_table_pointer, make_tail);
        memory_clear((byte*) page_table_pointer, sizeof(page_table), 0);
        return &get_page_table_pointer(dir->tablesPhysical[table_idx])->pages[address % 1024];
    } else return nullptr;
}

void* get_page_address(u_dword address) {
    page_pointer* ptr = get_page((u_dword) address, 0x00000003, current_directory);
    if (ptr) {
        *ptr = create_page_entry((const byte*) address, 0x00000003);
        return (u_dword*) get_page_pointer(*ptr);
    } else return nullptr;
}



void initialise_paging() {
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    u_dword page_pool_size = (memory_end - orbit_start) / page_size / 8;
    page_pool = k_malloc_aligned(page_pool_size, false);
    memory_clear((byte*) page_pool, page_pool_size, 0);

    u_dword page_table_pool_size = (page_dirs - page_tables) / sizeof(page_table) / 8;
    page_pool = k_malloc_aligned(page_table_pool_size, false);
    memory_clear((byte*) page_pool, page_table_pool_size, 0);

    // Let's make a page directory.
    kernel_directory = (page_directory*) k_malloc_aligned(sizeof(page_directory), true);
    memory_clear((byte*) kernel_directory, sizeof(page_directory), 0);

    for (u_dword j = 0; j < orbit_start; j += page_size)
        *get_page(j, 0x00000003, kernel_directory) = create_page_entry((const byte*) j, 0x00000003);
    for (u_dword j = memory_end; j < memory_edge; j += page_size)
        *get_page(j, 0x00000003, kernel_directory) = create_page_entry((const byte*) j, 0x00000003);

    // Before we enable paging, we must register our page fault handler.
    set_interrupt_handler(14, page_fault);

    current_directory = kernel_directory;

    // Now, enable paging!
    asm volatile("mov %0, %%cr3":: "r"(&current_directory->tablesPhysical));
    u_dword cr0, cr3;
    asm volatile("mov %%cr3, %0": "=r"(cr3));
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000001; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}



void page_fault(registers* regs) {
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    u_dword faulting_address;
    asm ("mov %%cr2, %0" : "=r" (faulting_address));

    // The error code gives us details of what happened.
    int present = regs->err_code & 0x1;      // Page not present
    int rw = regs->err_code & 0x2;           // Write operation?
    int us = regs->err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

    // Output an error message.
    bad("Page fault! ( ")
    if (present) bad("present ")
    if (!rw) bad("read-only ")
    if (us) bad("user-mode ")
    if (reserved) bad("reserved ")
    bad(") at %h (EIP: %h)\n", faulting_address, regs->eip)
    asm("jmp .");
}
