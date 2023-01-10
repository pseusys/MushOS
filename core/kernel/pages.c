#include "pages.h"

#include "../../lib/base/generic.h"
#include "../../lib/base/stdio.h"
#include "../../lib/base/memory.h"
#include "../../lib/base/heap.h"

#include "../drivers/screen.h"


#define paging_pool_start 0x100000
#define paging_pool_end 0xf00000

#define user_space_start 0x1000000
#define user_space_end 0x10000000

#define page_size 0x1000
#define full_byte 0xff


page_folder* kernel_directory, * current_directory;
void* page_dirs, * page_tables;
byte* paging_pool, * user_space_pool;


static u_dword get_free_bit(u_byte search) {
    if (search == full_byte) return search;
    for (u_dword i = 0; i < 8; ++i) if (!(search & (1u << i))) return i;
}

static void* get_contents_as_page(page_pointer page) {
    return (void*) (page.frame * page_size);
}

static page_pointer* get_contents_as_table(page_pointer page) {
    return (page_pointer*) (page.frame * page_size);
}


static void* allocate_frame() {
    for (u_dword i = 0; i < size_of(user_space_pool); ++i) {
        u_dword free_bit = get_free_bit(user_space_pool[i]);
        if (free_bit != full_byte) {
            user_space_pool[i] |= (1u << free_bit);
            return (void *) (user_space_start + (i * 8 + free_bit) * page_size);
        }
    }
    return nullptr; // TODO: raise out of RAM exception!
}

static page_folder* allocate_page_folder() {
    for (u_dword i = 0; i < size_of(paging_pool); ++i) {
        u_dword free_bit = get_free_bit(paging_pool[i]);
        if (free_bit != full_byte) {
            paging_pool[i] |= (1u << free_bit);
            void* address = (void *) (paging_pool_start + (i * 8 + free_bit) * page_size);
            memory_clear((byte*) address, page_size, 0);
            return address;
        }
    }
    return nullptr; // TODO: raise out of RAM exception!
}

static void free_frame(void* address) {
    u_dword index = (u_dword) address - user_space_start;
    user_space_pool[index / 8] &= ~(0x1 << (index % 8));
}

static void free_page_table(page_folder* table, boolean top_level) {
    u_dword index = (u_dword) table - paging_pool_start;
    paging_pool[index / 8] &= ~(0x1 << (index % 8));
    for (u_dword i = 0; i < pages_in_table; i++) {
        void* contents = get_contents_as_page(table->contents[i]);
        if (top_level) free_page_table((page_folder*) contents, false);
        else free_frame(contents);
    }
}


static page_pointer create_page_entry(const void* address, boolean is_kernel, boolean is_writeable) {
    page_pointer page_table;
    page_table.frame = (u_dword) address / page_size;
    page_table.user = is_kernel ? 0 : 1;
    page_table.rw = is_writeable ? 1 : 0;
    page_table.present = 1;
    return page_table;
}

static page_pointer* get_page(u_dword address, boolean make, page_folder *dir) {
    if (address > user_space_end) return nullptr; // 
    // Turn the address into an index.
    address /= page_size;
    // Find the page table containing this address.
    u_dword table_idx = address / pages_in_table;

    u_dword entry = (u_dword) get_contents_as_page(dir->contents[table_idx]);
    if (!entry) { // If this table is already assigned
        if (!make) return nullptr; // TODO: throw page doesn't exist exception.
        else dir->contents[table_idx] = create_page_entry((void*) allocate_page_folder(), true, true);
    }
    return &get_contents_as_table(dir->contents[table_idx])[address % pages_in_table];
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


void switch_page_directory(page_folder *dir) {
    u_dword cr0;
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->contents));
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void initialise_paging() {
    // The size of physical memory. For the moment we
    // assume it is 256MB big.
    u_dword paging_pool_size = (paging_pool_end - paging_pool_start) / page_size / 8;
    paging_pool = malloc(paging_pool_size);
    memory_clear((byte*) paging_pool, paging_pool_size, 0);

    u_dword user_space_pool_size = (user_space_end - user_space_start) / page_size / 8;
    user_space_pool = malloc(user_space_pool_size);
    memory_clear((byte*) user_space_pool, user_space_pool_size, 0);

    // Let's make a page directory.
    kernel_directory = (page_folder*) allocate_page_folder();
    memory_clear((byte*) kernel_directory, page_size, 0);
    current_directory = kernel_directory;

    for (u_dword j = 0; j < user_space_start; j += page_size)
        *get_page(j, true, kernel_directory) = create_page_entry((const byte*) j, true, false);

    // Before we enable paging, we must register our page fault handler.
    set_interrupt_handler(14, page_fault);

    // Now, enable paging!
    switch_page_directory(kernel_directory);
}
