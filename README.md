# MushOS

[![BUILD](https://github.com/pseusys/MushOS/actions/workflows/build.yml/badge.svg)](https://github.com/pseusys/MushOS/actions/workflows/build.yml)
[![TEST](https://github.com/pseusys/MushOS/actions/workflows/test.yml/badge.svg)](https://github.com/pseusys/MushOS/actions/workflows/test.yml)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/4018/badge)](https://bestpractices.coreinfrastructure.org/projects/4018)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=pseusys_MushOS&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=pseusys_MushOS)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

MushOS is a UNIX-like OS prototype for x86 32-bit protected mode, written from scratch in NASM assembly and C with no external runtime dependencies.
It implements the complete chain from the first BIOS instruction through virtual memory, preemptive multitasking, and hardware drivers, accompanied by **MushLib** — a fully freestanding standard library that replaces libc for both the kernel and future user-space programs.

The project was originally motivated by two questions: (1) how does software actually meet hardware at the lowest level, and (2) how can an allocator expose the size of a block to its caller without a separate lookup?
Both are answered concretely in the implementation.

## Design highlights

MushOS was built around two concrete research questions — not just "how do I make an OS" but:

1. **How does software actually meet hardware?** — traced all the way from which CPU instruction switches CPU modes (`or eax, 0x01 / mov cr0, eax`) to which port byte resets the PIC (0x20 → 0x20/0xA0) to which CR3 load activates a page directory.
2. **Why can't you ask an allocator how big an allocation is?** — treated as a solvable design problem; answered by the intrusive block-header heap in MushLib.

Several techniques in the implementation are unconventional.
The full rationale for each is in [docs/sources/design_notes.md](docs/sources/design_notes.md); brief summaries:

| Technique | Where | What is unusual |
| --- | --- | --- |
| Self-returning EIP reader | `utils.asm` | `pop eax; jmp eax` makes EIP look like a C return value — enables context switching in pure C without an assembly trampoline |
| `0xDEADBABA` re-entry sentinel | `task.c` | The context-switch epilogue sets EAX to a sentinel before jumping; the resumed task sees it as a return value and exits `switch_task` cleanly — no extra flag or field needed |
| Paging-disabled frame copy | `utils.asm` | `fork()` must copy physical frames that are not mapped in the current VA space; solution is to clear CR0.PG, copy, restore — a narrow window where physical and virtual addresses are the same |
| Heap as a research outcome | `lib/base/heap.c` | Designed explicitly to answer "how do you recover allocation size from a pointer?"; intrusive headers give `size_of(ptr)` in O(1) |
| Freestanding vararg | `lib/base/vararg.h` | Reads EBP+8+offset in inline assembly to walk the cdecl stack frame — no `<stdarg.h>`, ABI assumptions made explicit |
| Kernel-space exception system | `lib/base/exceptions.c` | Per-ID handler registration, typed exceptions, `throw_verbose` captures `__FILE__`/`__LINE__`; allocation exception has a special codepath because the handler cannot itself allocate |
| Build-time ELF section verifier | `scripts/check_kernel_sectors.py` | Uses `lief` to check each ELF section's virtual address and size against the memory map; fails the build before a broken image is produced |
| Syscall ABI before ring 3 | `lib/base/syscall.h` | `call_system` macro implements a complete argument-passing and register-save convention via software interrupt — already in use for VGA output, ready for ring 3 without changes |

## Architecture

```text
┌─────────────────────────────────────────────────────────────┐
│  MushLib  — freestanding stdlib: heap · string · format     │
│            exceptions · vararg · memory · math              │
├─────────────────────────────────────────────────────────────┤
│  Kernel   — IDT/PIC · virtual memory · multitasking         │
│             drivers (VGA · PS/2 · disk · VFS) · modules     │
├─────────────────────────────────────────────────────────────┤
│  Bootloader — 16-bit real mode → 32-bit protected mode      │
│               custom GDT · BIOS disk I/O · no GRUB          │
└─────────────────────────────────────────────────────────────┘
              runs on: QEMU · x86 bare metal
```

## Components

### Bootloader (`core/boot/loader.asm`)

A hand-written, 512-byte first-stage bootloader that handles the entire bring-up sequence with no external loader:

- Defines a flat GDT with two overlapping 4 GB code/data segments
- Reads 80 disk sectors via BIOS `int 13h`, loading the kernel image at `0x8000`
- Switches the CPU from 16-bit real mode to 32-bit protected mode via a far jump on the GDT code segment
- Sets up the initial stack, then calls the C kernel entry point

### Kernel (`core/kernel/`)

| Subsystem | Source | Description |
| --- | --- | --- |
| Entry | `kernel.c` | Ordered initialisation: heap → IDT → timer → screen → keyboard → modules → paging → tasking |
| Interrupts | `interruption_tables.c` | Full 256-entry IDT; handlers for all 32 CPU exceptions (ISR 0–31); 16 hardware IRQs remapped from the 8259 PIC to ISR 32–47; custom syscall vectors at ISR 48–52 |
| Virtual memory | `pages.c` | Two-level page directory/table structure; bitmap frame allocator over the user-space pool; page-fault handler with CR2 decoding; `clone_directory()` performs a deep copy of user pages for `fork()` while sharing kernel mappings |
| Multitasking | `task.c` | Preemptive round-robin scheduler; `fork()` clones the current page directory and appends a new `task` node; `switch_task()` saves/restores ESP, EBP, EIP in inline assembly using a sentinel value (`0xDEADBABA`) to detect re-entry; PIT at 100 Hz drives context switches |
| Timer | `timer.c` | Programmes the 8253/8254 PIT to divisor 11932 (≈ 100 Hz tick rate) |
| Module loader | `modules.c` | Loads raw binary modules from disk sectors into kernel address space at runtime |

### Drivers (`core/drivers/`)

| Driver | Description |
| --- | --- |
| `screen.c` | VGA text mode, 80×25 characters, 16 colours; cursor tracking via I/O ports `0x3D4`/`0x3D5`; scrolling and full-screen clear |
| `keyboard.c` | PS/2 keyboard via IRQ1; interrupt-driven; 51-entry scancode→ASCII table |
| `disk.asm` | Raw CHS sector access via BIOS `int 13h` |
| `vfs.c` | Virtual filesystem abstraction layer — `fs_node` struct with `read`/`write`/`open`/`close`/`readdir`/`finddir` interface signatures for future filesystem implementations |
| `ports_io.asm` | `port_byte_in` / `port_byte_out` primitives for direct hardware port access |

### MushLib (`lib/base/`)

A fully freestanding standard library compiled with `-nostdlib -ffreestanding -m32`.
All modules can be replaced at build time (see [Library Modules](#library-modules)).

| Module | Description |
| --- | --- |
| `heap.c` | Best-fit allocator with intrusive per-block headers; `size_of(ptr)` macro recovers block size from any allocation — directly addressing the "size from pointer" problem; `ralloc`, `zalloc`, `challoc`, `unalloc` |
| `format.c` | `printf`-style formatter: 8 conversion specifiers, width and precision, colour-coded output macros (`good`, `info`, `warn`, `error`) |
| `string.c` | `len`, `equals`, `concatenate`, `substring_beg/mid/end`, `first_pos`, `last_pos` |
| `exceptions.c` | Structured exception system: typed exception IDs, handler registration, `throw_verbose` captures `__FILE__` and `__LINE__` at the throw site |
| `vararg.c` | Variadic-argument support for the freestanding environment |
| `memory.c` | `memory_clear`, `memory_copy`, `memory_move`, `memory_set` |
| `generic.h` | Fixed-width type aliases (`u_byte`, `u_word`, `u_dword`, `u_qword`; signed equivalents; `real`/`precise`/`exact` for floats; `string`; `boolean`) |

## Memory Layout

Key address regions derived from the implementation — see [docs/sources/memory_map.md](docs/sources/memory_map.md) for the full annotated map.

| Address range | Contents |
| --- | --- |
| `0x0000 – 0x1FFF` | BIOS data area |
| `0x2000 – 0x7000` | Boot stack (grows down from `0x7000`, 20 KB) |
| `0x7C00 – 0x7DFF` | Boot sector (512 B, MBR) |
| `0x8000 – 0x11FFF` | Kernel image (80 sectors, ≤ 40 KB) |
| `0x50000 – 0xCFFFF` | Kernel heap (512 KB) |
| `0x100000 – 0xEFFFFF` | Paging pool — page directories and page tables |
| `0x1000000 – 0xFFFFFFF` | User space (256 MB, frame-allocated) |

## Build & Run

**Prerequisites:** `nasm`, `gcc-multilib`, `cmake ≥ 3.20`, `qemu-system-x86_64`, `python3`

```bash
# Build everything and launch in QEMU
make run

# Build MushLib only (no emulator required)
make build_lib

# Run the unit test suite (host-native, no emulator)
make test

# Generate Doxygen HTML documentation
make docs

# Remove all build artefacts
make clean
```

## Library Modules

MushLib is designed to be modular: any module can be replaced at build time by supplying a source file whose **name matches the base module** and whose functions match the signatures declared in `lib/base/`.
Example — substitute a custom heap implementation:

```bash
make C_SOURCES="path/to/your/heap.c;" build_lib
```

The same mechanism works for `.h` and `.asm` sources.
To replace multiple files, separate paths with semicolons; a trailing semicolon is always required.

## Testing

Unit tests cover the core MushLib components (`format`, `string`, `generic`, `vararg`) under a host-native build — no emulator needed:

```bash
make test   # builds MushTest, then runs ctest --verbose
```

GitHub Actions runs both the full kernel build and the test suite on every push.

## Requirements

| Tool | Purpose |
| --- | --- |
| `nasm` | Assembling bootloader and kernel `.asm` files |
| `gcc-multilib` | 32-bit cross-compilation of kernel and library C code |
| `cmake ≥ 3.20` | Build system |
| `qemu-system-x86_64` | Running the OS image |
| `python3` | Build scripts: header aggregation, kernel sector size validation |
| `doxygen` *(optional)* | Generating HTML documentation |

## Roadmap

- [x] Custom two-stage bootloader (real mode → protected mode, no GRUB)
- [x] Full 256-entry IDT with 8259 PIC remapping
- [x] VGA text-mode driver and interrupt-driven PS/2 keyboard driver
- [x] MushLib: heap, string, format, exceptions, vararg, memory, math
- [x] Two-level virtual memory with bitmap frame allocator
- [x] Preemptive round-robin multitasking with `fork()` and page-directory cloning
- [ ] Dynamic kernel memory map (detect RAM size at boot)
- [ ] Filesystem (MushFS) + initrd
- [ ] Interactive shell
- [ ] Audio driver
- [ ] User mode (ring 3 + syscall interface)
- [ ] Video mode
- [ ] Sample user-space applications
- [ ] Custom cross-compiler toolchain (remove host-gcc dependency)
