/**
 *           MUSH MEMORY MAP:
 *  ┌────────────────────────────────┐ 0x0
 *  │                                │
 *  │       BIOS interruptions       │
 *  │                                │
 *  ├────────────────────────────────┤ 0x1000
 *  │                                │
 *  │                                │
 *  │           Kernel Heap          │ See @code MushLib/heap
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x7000
 *  │                                │
 *  │          Kernel Stack          │
 *  │                                │
 *  ├────────────────────────────────┤ 0x7c00
 *  │                                │
 *  │      Boot sector and GDT       │ See @code MushCore/boot/loader
 *  │                                │
 *  ├────────────────────────────────┤ 0x8000
 *  │                                │
 *  │      Kernel data segment       │ See @code MushCore/Makefile
 *  │                                │
 *  ├────────────────────────────────┤ 0x9000
 *  │                                │
 *  │                                │
 *  │           Kernel code          │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x12000
 *  │                                │
 *  │         Placement new          │ See @code MushCore/kernel/placement
 *  │                                │
 *  ├────────────────────────────────┤ 0x20000
 *  │                                │
 *  │                                │
 *  │          Page tables           │ See @code MushCore/kernel/pages
 *  │  (0x40000 bytes -> 64 tables)  │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x60000
 *  │                                │
 *  │                                │
 *  │        Page directories        │ See @code MushCore/kernel/pages
 *  │   (0x20000 bytes -> 32 dirs)   │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x80000
 *  │                                │
 *  │                                │
 *  │      BIOS reserved memory      │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x100000
 *  │                                │
 *  │                                │
 *  │                                │
 *  │         Orbital slots          │ See @code MushCore/drivers/navigator
 *  │ (0x100000 bytes -> 32 drivers) │
 *  │                                │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x200000
 *  │                                │
 *  │                                │
 *  │                                │
 *  │                                │
 *  │           User space           │
 *  │                                │
 *  │                                │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0xf00000
 *  │                                │
 *  │                                │
 *  │         ISO memory gap         │
 *  │                                │
 *  │                                │
 *  ├────────────────────────────────┤ 0x1000000
 *  │                                │
 *  │        Unused by MushOS        │
 *  │                                │
 */
