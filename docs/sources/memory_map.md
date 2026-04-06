# Memory Map

![MushOS Memory Map](memory_map.png)

## Address Space Layout

The physical address space is divided into fixed regions established by the bootloader and kernel initialisation code.
All addresses are physical (paging is identity-mapped for the kernel range; user space is virtually addressed through the two-level page tables).

```text
Physical address       Size        Contents
─────────────────────────────────────────────────────────────────
0x00000000 – 0x00001FFF   8 KB    BIOS data area (IVT, BDA)
0x00002000 – 0x00006FFF  20 KB    Boot stack  (grows ↓ from 0x7000)
0x00007C00 – 0x00007DFF  512 B    Boot sector (MBR, first 512 bytes of disk)
0x00008000 – 0x000117FF  ~40 KB   Kernel image  (80 sectors loaded by bootloader)
0x0000A000              (entry)   kernel_gate entry point called by bootloader
0x00050000 – 0x000CFFFF 512 KB    Kernel heap   (initialised before any subsystem)
0x00100000 – 0x00EFFFFF  14 MB    Paging pool   (page directories and page tables)
0x01000000 – 0x0FFFFFFF 240 MB    User space    (frame-allocated via bitmap)
```

## Key Boundaries

### Boot stack (`0x2000 – 0x7000`)

Established by the bootloader before entering protected mode.
`stack_pointer = 0x7000`, `stack_size = 0x5000` (20 KB, grows downward).
The stack region is mapped as a user-accessible page so that future user-mode tasks can share the initial stack layout.

### Kernel heap (`0x50000 – 0xCFFFF`)

Initialised as the very first kernel subsystem — before interrupts, before paging.
This allows the allocator (`ralloc`/`zalloc`) to be used by every subsequent subsystem.
Size: `kernel_heap_size = 0x80000` (512 KB).
The heap uses a best-fit strategy with intrusive block headers; `size_of(ptr)` recovers the allocated size from any pointer.

### Paging pool (`0x100000 – 0xEFFFFF`)

Managed by `pages.c` as a bitmap of 4 KB frames.
Used exclusively for page directory and page table allocation (`allocate_page_folder()`).
The pool bitmap itself is allocated from the kernel heap at `initialise_paging()` time.

### User space (`0x1000000 – 0xFFFFFFF`)

256 MB of virtually-addressed user space, managed by a second bitmap frame allocator (`allocate_frame()`).
Each process receives its own page directory (cloned from the kernel directory by `clone_directory()` during `fork()`).
Kernel mappings are shared (same physical pointers); user pages are deep-copied.
