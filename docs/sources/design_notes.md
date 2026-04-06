# Design Notes

This document describes the non-obvious design decisions and experimental techniques in MushOS that set it apart from typical hobby OS tutorials.
The project started from two concrete questions that are usually glossed over in OS literature:

1. **How does software actually meet hardware?** — not in the abstract sense of "memory-mapped I/O", but concretely: which CPU instruction switches modes, which register encodes the page directory, which port byte resets the PIC?
2. **Why can't you ask an allocator how big an allocation is?** — `malloc` takes a size but doesn't give one back; this was a deliberate, researchable design problem rather than a limitation to accept.

The sections below document the answers found, along with other techniques explored along the way.

---

## 1. Context switching without an assembly trampoline

Most tutorial OSes implement `switch_task` as a large assembly function.
MushOS keeps the scheduler in C and uses two x86-specific tricks instead.

### The self-returning EIP reader (`utils.asm:41`)

```nasm
get_instruction_pointer:
  pop eax
  jmp eax
```

On x86 cdecl, `call get_instruction_pointer` pushes the return address onto the stack.
This function pops that address into EAX (which is also the C integer return register) and jumps to it.
From the caller's perspective it looks exactly like a normal function that returned the value of EIP — the instruction pointer right after the call site.

This lets `task.c` capture the current EIP in pure C:

```c
u_dword eip = get_instruction_pointer();
```

No assembly trampoline, no linker magic.

### The `0xDEADBABA` re-entry sentinel (`task.c:100`)

After a context switch the resumed task will return from `get_instruction_pointer()` a second time (because the resumed EIP is the instruction after the original call).
To distinguish the two returns, the context-switch epilogue loads `0xDEADBABA` into EAX immediately before jumping to the new task:

```c
asm volatile(
    "mov $0xDEADBABA, %%eax;\n"
    "jmp *%%ecx;\n"
    ...
);
```

Since EAX is the return register, the resumed task's `get_instruction_pointer()` appears to have returned `0xDEADBABA`.
The check at the top of `switch_task` catches this and returns immediately:

```c
if (eip == 0xDEADBABA) return;
```

No flag register bits, no per-task re-entry field, no extra stack slot.
The sentinel value travels through the CPU's own return-value convention.

---

## 2. Physical frame copy with paging temporarily disabled (`utils.asm:5`)

`fork()` needs to deep-copy a physical 4 KB page frame into a freshly-allocated frame.
The problem: the destination frame is not mapped in the current page directory (it was just allocated from the frame pool).
Virtual memory is active at this point — there is no identity mapping for arbitrary physical addresses.

The solution is to temporarily disable paging at the CPU level, perform the copy using physical addresses, then re-enable paging:

```nasm
mov edx, cr0
and edx, 0x7fffffff   ; clear CR0.PG
mov cr0, edx          ; paging off

; ... copy 1024 dwords (4096 bytes) from source to dest ...

mov edx, cr0
or  edx, 0x80000000   ; set CR0.PG
mov cr0, edx          ; paging on
```

Interrupts are disabled for the duration (`cli` / `popf`) so no interrupt handler runs while the TLB is in an inconsistent state.
This technique requires understanding that CR0.PG is a global CPU flag, not a per-process setting, and that the code itself must remain accessible during the window — which it is, because the kernel is identity-mapped.

---

## 3. Heap design as a research outcome

The standard C allocator interface has an asymmetry: `malloc(n)` takes a size, but nothing gives it back.
Any code that needs to track allocation sizes must carry the size separately or use wrapper structs.

MushLib's heap is designed around eliminating this asymmetry.
Every allocation is preceded by an intrusive `heap_block_header` in memory:

```c
typedef struct {
    u_dword size;
    void* previous, * next;
} heap_block_header;
```

`ralloc(n)` returns a pointer to the memory _after_ the header.
`size_of(ptr)` subtracts `sizeof(heap_block_header)` from the pointer, casts, and reads the `size` field — O(1), no lookup table.

The headers also form a doubly-linked free list for the best-fit allocator.
`challoc` (resize) can extend in-place if the gap to the next block is large enough, avoiding a copy.

The allocation exception path has a deliberate special case: if the heap itself is full, the exception handler cannot print a message (that would require allocating a formatted string).
The kernel registers a minimal fallback via `handle_exceptions(allocation_exception_id, handle_allocation_exception)` that calls `terminate()` directly, sidestepping the formatter.

---

## 4. Variadic arguments without `<stdarg.h>`

The freestanding compilation flags (`-nostdlib -ffreestanding`) make `<stdarg.h>` unavailable.
MushLib implements variadic argument access by directly reading the x86 cdecl stack frame in inline assembly:

```c
#define get_arg(skip, type) ({         \
    type* arg;                          \
    asm volatile(                       \
        "add %%ebp, %1\n"              \
        "mov %1, %0"                   \
        : "=r"(arg): "r"(8 + skip)    \
    );                                  \
    *arg;                               \
})
```

On cdecl, EBP+8 is the first argument (EBP+0 = saved EBP, EBP+4 = return address).
`init_vararg(sizeof(last_named_arg))` aligns the offset to a 4-byte boundary, then `get_vararg(offset, type)` reads and advances.

The technique is intentionally brittle — it assumes the cdecl calling convention and 32-bit alignment — but it works for the target (x86, gcc -m32) and makes the dependency explicit rather than hiding it behind a compiler abstraction.

---

## 5. Structured exception handling in kernel space

Kernel code typically panics on unexpected conditions and gives little context.
MushLib provides a structured exception system that brings application-level error handling idioms into ring 0.

Exceptions are typed values:

```c
typedef struct {
    u_byte id;
    string type;
    string message;
    string file;
    u_dword line;
} exception;
```

Any subsystem can register a handler for a specific exception ID:

```c
handle_exceptions(allocation_exception_id, my_handler);
```

The `throw_verbose` macro captures the throw site at compile time:

```c
#define throw_verbose(id, type, message) {          \
    exception exc = {id, type, message, __FILE__, __LINE__}; \
    throw_exception(exc);                           \
}
```

If no handler is registered the default path prints the exception ID, type, message, and `file:line` in red, then calls `terminate()`.
This makes kernel-level bugs self-describing rather than presenting a blank screen or a raw register dump.

The system is designed for future user-mode use: each user process can install its own handlers (or rely on the default), and the kernel can terminate a misbehaving process without a full panic.

---

## 6. Build-time ELF section verifier

The bootloader reads exactly 80 sectors from disk and loads the kernel at physical address `0x8000`.
If the compiled kernel grows beyond its allocated section budgets, it silently overwrites adjacent memory regions at runtime — a class of bug that is difficult to diagnose.

`scripts/check_kernel_sectors.py` solves this at build time using the `lief` binary analysis library.
After compilation, the script is invoked for each ELF section to verify:

- The section is present in the output binary
- Its virtual address matches the expected value in the memory map
- Its size is strictly less than the allocated budget

```txt
Section    Physical   Virtual   Max size
.GOT.PLT   0x0000     0x8000    0x0100
.DATA      0x0100     0x8100    0x0400
.BSS       0x0500     0x8500    0x1000
.RODATA    0x1500     0x9500    0x0B00
.TEXT      0x2000     0xA000    0x26000
```

If any check fails, the build fails with a precise error message before an image is produced.
This turns a class of hard runtime bugs into a fast, deterministic build-time error — a technique borrowed from safety-critical embedded toolchains and not commonly seen in hobby OS projects.

---

## 7. Syscall ABI implemented as macros before ring 3 exists

User mode is not yet implemented, but the syscall calling convention is already defined and testable.
`syscall.h` provides `call_system` and `call_system_ret` macros that implement a complete convention:

1. Save all registers (`pusha`)
2. Collect variadic arguments into a heap-allocated `u_dword` array
3. Push arguments onto the stack in reverse order
4. Set up a fresh EBP frame and fire a software interrupt (`int N`)
5. Clean the stack, restore registers (`popa`)

The handler side uses `system_get_arg(stack_base, n, ret)` to read the nth argument from the saved frame.

Currently this is used by `stdio.h`'s `print_formatted` macro (ISR 48 drives VGA output).
When ring 3 is added, the same ABI will apply without changes to either side.
Defining the convention early forces correctness about the register-save protocol and argument layout before user/kernel boundary concerns make debugging harder.

---

## 8. MushLib as a swappable interface specification

The files under `lib/base/` define _interfaces_, not just implementations.
Any module can be replaced at build time by providing a source file of the same name:

```bash
make C_SOURCES="custom/heap.c;" build_lib
```

CMake substitutes the replacement, compiles it with the same freestanding flags, and links the result as `libmushlib.a`.
The kernel and the rest of MushLib continue to compile unchanged against the same headers.

This design makes MushLib a testbed for alternative algorithms: a different heap strategy (slab, buddy, pool), a different string representation (length-prefixed instead of null-terminated), or a hardware-specific memory backend can each be tested independently without forking the OS.

The `scripts/build_mushlib_header.py` script generates a single amalgamated `mushlib.h` from all individual module headers — stripping include guards and `#include` directives and inserting section comments — analogous to glibc's `<bits/...>` umbrella headers.

---

## 9. ISR 49 as a software-triggered debug probe

The debug interrupt handler (registered on ISR 49) can be triggered by any kernel code with `int 49` and dumps the full machine state at that point:

- All 8 general-purpose registers (EAX–EDX, ESI, EDI, EBP, ESP)
- EIP at the interrupt site
- 10 consecutive dwords from the stack ("orbital args") useful for inspecting call-site context

This is a self-hosted debugging mechanism: no external debugger or QEMU monitor command is needed to inspect state.
Combining it with the exception system (which captures `__FILE__`/`__LINE__`) gives a lightweight "printf debugging" capability that works at interrupt time.
