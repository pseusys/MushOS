#include "pages.h"
#include "../mushlib/logger.h"
#include "../mushlib/memory.h"
#include "../drivers/ports_io.h"
#include "interruptions.h"

// A bitset of frames - used or free.
u_dword *frames;
u_dword nframes;
page_directory_t* current_directory, * kernel_directory;
extern u_dword placement_address;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

// Static function to set a bit in the frames bitset
static void set_frame(u_dword frame_addr)
{
    u_dword frame = frame_addr/0x1000;
    u_dword idx = INDEX_FROM_BIT(frame);
    u_dword off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u_dword frame_addr)
{
    u_dword frame = frame_addr/0x1000;
    u_dword idx = INDEX_FROM_BIT(frame);
    u_dword off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static u_dword test_frame(u_dword frame_addr)
{
    u_dword frame = frame_addr/0x1000;
    u_dword idx = INDEX_FROM_BIT(frame);
    u_dword off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u_dword first_frame()
{
    u_dword i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                u_dword toTest = 0x1 << j;
                if ( !(frames[i]&toTest) )
                {
                    return i*4*8+j;
                }
            }
        }
    }
}



// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return; // Frame was already allocated, return straight away.
    }
    else
    {
        u_dword idx = first_frame(); // idx is now the index of the first free frame.
        if (idx == (u_dword)-1)
        {
            // PANIC is just a macro that prints a message to the screen then hits an infinite loop.
            b("No free frames!")
        }
        set_frame(idx*0x1000); // this frame is now ours!
        page->present = 1; // Mark it as present.
        page->rw = (is_writeable)?1:0; // Should the page be writeable?
        page->user = (is_kernel)?0:1; // Should the page be user-mode?
        page->frame = idx;
    }
}

// Function to deallocate a frame.
void free_frame(page_t *page)
{
    u_dword frame;
    if (!(frame=page->frame))
    {
        return; // The given page didn't actually have an allocated frame!
    }
    else
    {
        clear_frame(frame); // Frame is now free again.
        page->frame = 0x0; // Page now doesn't have a frame.
    }
}



extern byte sentinel;

void initialise_paging()
{
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    placement_address = (u_dword) &sentinel;
    const char* msg = "Placement address is: ";
    bn(msg, HEXADECIMAL, POINTER)endl()
    im(msg)in(placement_address, HEXADECIMAL, POINTER)endl()
    u_dword mem_end_page = 0x1000000;

    nframes = mem_end_page / 0x1000;
    frames = (u_dword*) k_malloc(INDEX_FROM_BIT(nframes));
    memory_clear((byte*) frames, INDEX_FROM_BIT(nframes), 0);

    // Let's make a page directory.
    kernel_directory = (page_directory_t*) k_malloc_aligned(sizeof(page_directory_t), true);
    memory_clear((byte*) kernel_directory, sizeof(page_directory_t), 0);
    current_directory = kernel_directory;

    set_interrupt_handler(14, page_fault);

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
        alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
        i += 0x1000;
    }
    // Before we enable paging, we must register our page fault handler.
    set_interrupt_handler(14, page_fault);

    // Now, enable paging!
    //switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    u_dword cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u_dword address, int make, page_directory_t *dir)
{
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    u_dword table_idx = address / 1024;
    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else if(make)
    {
        u_dword tmp;
        dir->tables[table_idx] = (page_table_t*) k_malloc_physical(sizeof(page_table_t), false, &tmp);
        memory_clear((byte*) dir->tables[table_idx], 0x1000, 0);
        bn(dir->tables[table_idx], HEXADECIMAL, POINTER)endl()
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
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
    bb()bm("Page fault! ( ")
    if (present) {bm("present ")}
    if (rw) {bm("read-only ")}
    if (us) {bm("user-mode ")}
    if (reserved) {bm("reserved ")}
    bm(") at ")bn(faulting_address, HEXADECIMAL, POINTER)endl()
}
