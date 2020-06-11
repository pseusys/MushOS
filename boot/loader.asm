[bits 16]
[org 0x7c00]

start:
    jmp kernel_start

KERNEL_OFFSET equ 0x1000


gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start



CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start



read_disk:
    pusha
    push dx

    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

    int 0x13

    jc .disk_error

    pop dx
    cmp dh, al
    jne .disk_error
    jmp .done

.disk_error:
    mov bx, ERROR_MSG
    call write_string
    stc

.done:
    popa
    ret


write_string:
    pusha
    mov ah, 0x0e

.repeat:
    lodsb
    cmp al, 0x00
    je .done
    int 0x10
    jmp .repeat

.done:
    popa
    ret



kernel_start:
    mov [BOOT_DRIVE], dl

    cli
    mov ax, cs
    mov ss, ax
    mov sp, start
    mov bp, start
    sti

    mov ds, ax
    mov es, ax

    mov si, BOOT_MSG
    call write_string

    mov dh, 15
    mov dl, [BOOT_DRIVE]
    mov bx, KERNEL_OFFSET
    call read_disk

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    jmp CODE_SEG:kernel_launch



[bits 32]

kernel_launch:

    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, start
    mov esp, start

    call KERNEL_OFFSET

    jmp $



variables:
    BOOT_DRIVE db 0
    BOOT_MSG   db "MUSHOS booted in real mode...", 0x0d, 0x0a, 0
    ERROR_MSG  db "Could not load second boot loader!", 0x0d, 0x0a, 0

magic_numbers:
    times 510-($-$$) db 0
    dw 0xaa55
