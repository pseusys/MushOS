[bits 32]
[extern _start] ; External C function from kernel.c
                ; It may be not the first function in kernel, so we ensure to launch the right one.

call _start ; Calling function.

jmp $ ; Hanging CPU up. TODO replace with hlt.
