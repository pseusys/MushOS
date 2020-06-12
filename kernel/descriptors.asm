;[bits 32]
;global gdt_flush    ; Allows the C code to call gdt_flush().
;global get_cs
;global idt_flush

;gdt_flush:
;    mov edx, [esp+8]
;    mov ecx, [esp+16]
;    mov eax, [esp+24]  ; Get the pointer to the GDT, passed as a parameter.
;    lgdt [eax]        ; Load the new GDT pointer
                      ; 0x10 is the offset in the GDT to our data segment
;    mov ds, dx        ; Load all data segment selectors
;    mov es, dx
;    mov fs, dx
;    mov gs, dx
;    mov ss, dx

;    mov cs, cx
;    jmp far [.flush]   ; 0x08 is the offset to our code segment: Far jump!

;.flush:
;    ret



;get_cs:
;    mov ax, cs
;    ret



;idt_flush:
;    push ebp
;    mov ebp, esp

;    mov eax, [ebp+8]  ; Get the pointer to the IDT, passed as a parameter.
;    lidt [eax]        ; Load the IDT pointer.

;    mov esp, ebp
;    pop ebp
;    ret



;%macro ISR_NO_ERROR_CODE 1  ; define a macro, taking one parameter
;  global isr%1        ; %1 accesses the first parameter.
;  isr%1:
;    cli
;    push byte 0
;    push byte %1
;    jmp isr_common_stub
;%endmacro

;%macro ISR_ERROR_CODE 1
;  global isr%1
;  isr%1:
;    cli
;    push byte %1
;    jmp isr_common_stub
;%endmacro

;ISR_NO_ERROR_CODE 0
;ISR_NO_ERROR_CODE 1
;ISR_NO_ERROR_CODE 2
;ISR_NO_ERROR_CODE 3
;ISR_NO_ERROR_CODE 4
;ISR_NO_ERROR_CODE 5
;ISR_NO_ERROR_CODE 6
;ISR_NO_ERROR_CODE 7
;ISR_ERROR_CODE 8
;ISR_NO_ERROR_CODE 9
;ISR_ERROR_CODE 10
;ISR_ERROR_CODE 11
;ISR_ERROR_CODE 12
;ISR_ERROR_CODE 13
;ISR_ERROR_CODE 14
;ISR_NO_ERROR_CODE 15
;ISR_NO_ERROR_CODE 16
;ISR_NO_ERROR_CODE 17
;ISR_NO_ERROR_CODE 18
;ISR_NO_ERROR_CODE 19
;ISR_NO_ERROR_CODE 20
;ISR_NO_ERROR_CODE 21
;ISR_NO_ERROR_CODE 22
;ISR_NO_ERROR_CODE 23
;ISR_NO_ERROR_CODE 24
;ISR_NO_ERROR_CODE 25
;ISR_NO_ERROR_CODE 26
;ISR_NO_ERROR_CODE 27
;ISR_NO_ERROR_CODE 28
;ISR_NO_ERROR_CODE 29
;ISR_NO_ERROR_CODE 30
;ISR_NO_ERROR_CODE 31



;extern isr_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
;isr_common_stub:
;   pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
;
;   mov ax, ds               ; Lower 16-bits of eax = ds.
;   push eax                 ; save the data segment descriptor
;
;   mov ax, 0x10  ; load the kernel data segment descriptor
;   mov ds, ax
;   mov es, ax
;   mov fs, ax
;   mov gs, ax

;   call isr_handler

;   pop eax        ; reload the original data segment descriptor
;   mov ds, ax
;   mov es, ax
;   mov fs, ax
;   mov gs, ax

;   popa                     ; Pops edi,esi,ebp...
;   add esp, 8     ; Cleans up the pushed error code and pushed ISR number
;   sti
;   iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
