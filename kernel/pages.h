#ifndef MUSHOS_PAGES_H
#define MUSHOS_PAGES_H

#include "generic.h"
#include "interruption_tables.h"

typedef struct page {
    u_dword present    : 1;   // Page present in memory
    u_dword rw         : 1;   // Read-only if clear, readwrite if set
    u_dword user       : 1;   // Supervisor level only if clear
    u_dword accessed   : 1;   // Has the page been accessed since last refresh?
    u_dword dirty      : 1;   // Has the page been written to since last refresh?
    u_dword unused     : 7;   // Amalgamation of unused and reserved bits
    u_dword frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table {
    page_t pages [1024];
} page_table_t;

typedef struct page_directory {
    /**
       Array of pointers to pagetables.
    **/
    //page_table_t* tables[1024];
    /**
       Array of pointers to the pagetables above, but gives their *physical*
       location, for loading into the CR3 register.
    **/
    struct page_table* tablesPhysical[1024];
    /**
       The physical address of tablesPhysical. This comes into play
       when we get our kernel heap allocated and the directory
       may be in a different location in virtual memory.
    **/
    //u_dword physicalAddr;
} page_directory_t;

/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void initialise_paging();

/**
  Causes the specified page directory to be loaded into the
  CR3 register.
**/
void switch_page_directory(page_directory_t *new);

/**
  Retrieves a pointer to the page required.
  If make == 1, if the page-table in which this page should
  reside isn't created, create it!
**/
page_t* get_page(u_dword address, boolean make, page_directory_t *dir);

u_dword* get_page_address(u_dword address);

/**
  Handler for page faults.
**/
void page_fault(registers* regs);

#endif //MUSHOS_PAGES_H
