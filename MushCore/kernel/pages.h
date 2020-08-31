#ifndef MUSHOS_PAGES_H
#define MUSHOS_PAGES_H

#include "generic.h"
#include "interruption_tables.h"

#define page_size 0x1000

typedef struct {
    u_dword present    : 1;   // Page present in memory
    u_dword rw         : 1;   // Read-only if clear, readwrite if set
    u_dword user       : 1;   // Supervisor level only if clear
    u_dword write      : 1;   // Write through?
    u_dword cache      : 1;   // Cache disabled?
    u_dword accessed   : 1;   // Has the page been accessed since last refresh?
    u_dword dirty      : 1;   // Has the page been written to since last refresh?
    u_dword zero       : 1;   // Always zero
    u_dword global     : 1;   // Global?
    u_dword unused     : 3;   // Amalgamation of unused and reserved bits
    u_dword frame      : 20;  // Frame address (shifted right 12 bits)
} page_pointer;



typedef struct {
    page_pointer pages [1024];
} page_table;

typedef struct {
    u_dword present    : 1;   // Page present in memory
    u_dword rw         : 1;   // Read-only if clear, readwrite if set
    u_dword user       : 1;   // Supervisor level only if clear
    u_dword write      : 1;   // Write through?
    u_dword cache      : 1;   // Cache disabled?
    u_dword accessed   : 1;   // Has the page been accessed since last refresh?
    u_dword zero       : 1;   // Always zero
    u_dword size       : 1;   // 0 if 4kb, 1 if 4 mb
    u_dword ignored    : 1;   // Just ignored
    u_dword unused     : 3;   // Amalgamation of unused and reserved bits
    u_dword frame      : 20;  // Frame address (shifted right 12 bits)
} page_table_pointer;



typedef struct {
    page_table_pointer tablesPhysical[1024];
} page_directory;



/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void initialise_paging();

void* get_page_address(u_dword address);

/**
  Handler for page faults.
**/
void page_fault(registers* regs);

#endif //MUSHOS_PAGES_H
