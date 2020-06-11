; This code lies in the first sector of hard disk, the boot sector.
; BIOS works in 16-bit (real) CPU mode, as soon as it finds boot sector, it calls it.

[bits 16] ; Code also compiled as 16-bit, BIOS-compatible.
[org 0x7c00] ; Code is loaded at 0x7c00 address in RAM - special booting address.

start:
    jmp kernel_start ; Jumping to actual code to avoid executing GDT-table.

KERNEL_OFFSET equ 0x100000 ; Offset in RAM, where kernel will be loaded, it is also defined in linker.
                           ; I've been said all big kernels are loaded there. Haven't proved.


gdt_start: ; Beginning of GDT-table. The table itself contains information about RAM segments.
           ; Each segment has a set of parameters (e.g. readable/writable, data/code, etc.)
           ; This table is a basic flat model table with no security required to go to 32-bit CPU mode.
           ; The table contains 2 overlapping 4GB segments (code and data). Since they overlap, they're not protected.
           ; It maybe will be changed later, from protected mode along with other interesting features.

gdt_null: ; First sector has to be null. For catching segment-register-not-set error catching.
    dd 0x0 ; Info about one segment takes 4 words (8 bytes) of memory.
    dd 0x0 ; This null-segment is apparently initialized with all zeroes.

gdt_code: ; The code segment.
    dw 0xffff ; Segment limit - how long is it. 4GB, as stated.
    dw 0x0000 ; Segment base - where it begins in memory. In the very beginning.
    db 0x00 ; Segment base one more time (for some reason it has to be initialized two times).
    db 10011010b ; Segment flags, these are:
                    ; Present (1) - the segment is present.
                    ; Privilege (00) - highest privilege.
                    ; Descriptor type (1) - 1 for code and data, 0 for traps.
                 ; Type flags:
                    ; Type (1) - 1 for code, 0 for data.
                    ; Conforming (0) - means lower-privileged segments can't call it.
                    ; Readable (1) - readable or executable-only.
                    ; Accessed (0) - CPU sets when accesses the segment.
    db 11001111b ; Segment flags:
                    ; Granularity (1) - Expands limit from 0xffff to 0xffff000.
                    ; 32 bit (1) - set 0 for 64-bit code.
                    ; 64 bit (0) - we have it unused.
                    ; AVL (0) - flag for custom uses.
                 ; Other 4 bits (1111) contain info about segment limit.
    db 0x00 ; Segment base (for the third time, yes).

gdt_data: ; The data segment (I will comment differences only).
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b ; Type (0) - for data.
    db 11001111b
    db 0x00

gdt_end: ; Label marks end of GDT table in memory.

gdt_descriptor: ; Special structure - GDT descriptor is used to transfer info about GDT table.
    dw gdt_end - gdt_start - 1 ; Size of GDT table, one less than actual.
    dd gdt_start ; Position of table in memory.



CODE_SEG equ gdt_code - gdt_start ; These are the values that CS and DS registers must contain in protected mode.
DATA_SEG equ gdt_data - gdt_start ; They point to beginning of special segments in GDT.



read_disk: ; Procedure for reading sectors from disc into memory.
           ; DH = how many sectors; DL = from what drive; BX = where to load.
    pusha ; Push all registers.
    push dx ; For later error checking.

    mov ah, 0x02 ; int 13h : 02 - reading from disk BIOS function.
    mov al, dh ; Sectors number.
    mov ch, 0x00 ; Track cylinder.
    mov dh, 0x00 ; Track number.
    mov cl, 0x02 ; First sector to read.

    int 13h

    jc .disk_error ; int 13h sets -c flag if reading error occurs.

    pop dx ; int 13h stores number of sectors read into al.
    cmp dh, al ; Checking how many sectors were read vs. how many must've been read.
    jne .disk_error ; If not the same number handling error.
    jmp .done

.disk_error:
    mov si, ERROR_MSG ; Setting and printing error message.
    call write_string
    stc ; Ensuring -c flag was set to check if function finished successfully.

.done:
    popa ; Pop all registers.
    ret


write_string: ; Procedure for writing string into console.
              ; SI = pointer to string in ASCIIZ format.
    pusha
    mov ah, 0x0e ; int 10h : 0e - writing char into console and setting cursor to next position BIOS function.
                 ; Other parameters (color, mode, etc.) are set by default.

.repeat:
    lodsb ; Loading char from SI to AL, increasing SI.
    cmp al, 0x00 ; Checking if the end of string was reached.
    je .done ; Escaping cycle.
    int 10h
    jmp .repeat ; Proceeding to next char.

.done:
    popa
    ret



kernel_start: ; Start of booting code.
    mov [BOOT_DRIVE], dl ; BIOS puts number of booting device into DL. Saving it into variable.

    cli ; Clearing interruptions before SP and SP modifying.
    mov ax, cs ; CS points to 7c00 at the boot time, the current segment. TODO: check!!
    mov ss, ax ; Placing stack to the same segment.
    mov sp, start ; Stack grows downwards in memory, while the code - upwards. TODO: check!!
    mov bp, start ; So stack starts right in the entry point, but grows to another direction.
    sti ; Setting interruptions back.

    mov ds, ax ; Setting DS and ES to the same segment.
    mov es, ax ; My boot loader contains code and variables all together.

                 ; Setting default output video mode to color VGA 80x25.
    mov ah, 0x00 ; int 10h : 00 - setting video mode BIOS function.
    mov al, 0x03 ; Color, 80x25, text, memory at 0xb8000 preset.
    int 10h

    mov si, BOOT_MSG ; Setting and printing boot message.
    call write_string

    mov dh, 0x09 ; Loading 15 sectors from disk into memory. It's enough for our kernel. TODO: compile-time setting.
    mov dl, [BOOT_DRIVE] ; Setting current boot drive.
    mov bx, KERNEL_OFFSET ; Setting kernel position to one defined above.
    call read_disk

    jc .loop ; If reading is not successful hanging up system.

    cli ; Clearing interruptions - the interruptions in protected mode differ.
    lgdt [gdt_descriptor] ; Loading defined above GDB table.
    mov eax, cr0 ; cr0 CPU register contains information about processor mode in last byte.
    or eax, 0x01 ; Setting CPU mode to protected.
    mov cr0, eax ; Loading cr0 register back.

    jmp CODE_SEG:kernel_launch ; Performing long jump to stop all parallel tasks (piping) and force CPU into protected mode.

.loop:
    jmp $ ; Hanging CPU up. TODO replace with hlt.



[bits 32] ; This segment is compiled in 32-bits mode to be executed in protected CPU mode.

kernel_launch: ; Long jump sets CS to CODE_SEG automatically at this point.

    mov ax, DATA_SEG ; Setting other registers to DATA_SEG.
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax ; Even some strange-looking, protected mode registers.
    mov gs, ax

    mov ebp, start ; Resetting stack back where it was.
    mov esp, start

    call KERNEL_OFFSET ; Calling pre-loaded C kernel (kernel_gate).

    jmp $ ; Hanging CPU up. TODO replace with hlt.



variables:
    BOOT_DRIVE db 0
    BOOT_MSG   db "MUSHOS booted in real mode...", 0x0d, 0x0a, 0
    ERROR_MSG  db "Could not load second boot loader!", 0x0d, 0x0a, 0

magic_numbers:            ; $$ - special label in NASM, I'm not 100% sure how it works.
    times 510-($-$$) db 0 ; Filling least of boot segment with zeroes not to be executed for any reason.
    dw 0xaa55 ; The last two bytes are special "magic number" allowing BIOS to differ boot sector from regular.
