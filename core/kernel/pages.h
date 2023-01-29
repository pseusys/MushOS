#ifndef MUSHOS_PAGES_H
#define MUSHOS_PAGES_H

#include "../../lib/base/generic.h"

#include "interruption_tables.h"


#define pages_in_table 1024

#define page_size 0x1000
#define full_byte 0xff


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
    page_pointer contents[pages_in_table];
} page_folder;


/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void initialise_paging();

/**
  Handler for page faults.
**/
void page_fault(registers* regs);

#endif //MUSHOS_PAGES_H
