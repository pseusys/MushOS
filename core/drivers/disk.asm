; // TODO: This code only works for the 286+ so you must detect for 8088's somewhere in your code.

[bits 32] ; Function for reading disk contents to memory
global read_disk ; Function using legacy PIO mode + polling, should be used early during boot only!


read_disk: ; First parameter (pointer): [ESP + 8] - LBA address of the first sector to read.
           ; Second parameter (boolean): [ESP + 12] - Master or slave drive to use. // TODO: make use of it, setup drive check!
           ; Third parameter (word): [ESP + 16] - Bytes number to read (must be even).
           ; Fourth parameter (word): [ESP + 20] - Offset to read (must be even).
           ; Last parameter (pointer): [ESP + 24] - Pointer to RAM where to load data from disk.
    push ebp ; Save stack base.
    mov ebp, esp ; Set register base to register top.
                 ; Function won't be able to interfere with previous stack entries.
    pusha ; Save all registers. They will be restored afterwards.

    ; Calculate number of sectors to read (ceil).
    mov ebx, [ebp+16] ; Store offset in ebx.
    add ebx, [ebp+20] ; Add length to ebx - ceil(ebx) will be number of sectors to read.
    shl ebx, 9 ; Divide ebx by sector size.
    test ebx, 0x1ff ; Calculate eax by sector size division remainder. // TODO: check sector size!
    jz .setup_reading ; Setup reading if remainder equals zero.
    add ebx, 1 ; Add 1 to ceil otherwise.


.setup_reading:
    mov edx, 0x01F6 ; Port to send drive and bit 24 - 27 of LBA.
    mov eax, [ebp+8] ; Copy LBA to eax.
    shr eax, 24 ; Get bit 24 - 27 in al.
    or al, 0xe0 ; Set bit 6 in al for LBA mode.
    out dx, al ; Send eax value to port.
 
    mov edx, 0x01f2 ; Port to send number of sectors.
    mov al, bl ; Get number of sectors from ebx.
    out dx, al 
 
    mov edx, 0x1f3 ; Port to send bit 0 - 7 of LBA.
    mov eax, [ebp+8] ; Copy LBA to eax.
    out dx, al

    mov edx, 0x1f4 ; Port to send bit 8 - 15 of LBA.
    mov eax, [ebp+8] ; Copy LBA to eax.
    shr eax, 8 ; Get bit 8 - 15 in al.
    out dx, al
 
    mov edx, 0x1f5 ; Port to send bit 16 - 23 of LBA.
    mov eax, [ebp+8] ; Copy LBA to eax.
    shr eax, 16 ; Get bit 16 - 23 in al.
    out dx, al
 
    mov edx, 0x1f7 ; Command port.
    mov al, 0x20 ; Read with retry command.
    out dx, al


.busy:
    in al, dx ; Read status port.
    test al, 0x80 ; Check for BSY status flag.
    jz .ready ; Start reading if flag isn't set.

    test al, 0x1 ; Check for ERR flag.
    jnz .general_error ; Jump to general error handler if it's set.
    test al, 0x20 ; Check for DF flag.
    jnz .drive_fault_error ; Jump to drive fault handler if it's set.

    jmp .busy ; Loop otherwise.

.general_error:
    mov dx, 0x1f1 ; Port to receive error message.
    in ax, dx ; Read error code from port.
    jmp .return

.drive_fault_error:
    mov ax, 0x10 ; Drive fault doesn't have error code, so we'll set it to first bit of second byte.
    jmp .return


.ready:
    in al, dx ; Read status port.
    test al, 0x8 ; Check for DRQ status flag.
    jz .ready ; Loop some more if flag isn't set.
    mov dx, 0x1f0 ; Data port, data comes in and out of here.
                  ; NB! This is a word port only. We can't read a single byte from here. Only a word.

    mov ecx, [ebp+20] ; Copy offset to ecx.
    shr ecx, 1 ; Divide offset by 2 - calculate offset in words.
    cmp ecx, 0 ; If offset equals zero start reading data.
    je .write_data

.dump_offset:
    in ax, dx ; If offset doesn't equal zero, drain offset data.
    loop .dump_offset

.write_data:
    mov ecx, [ebp+16] ; Copy data length to ecx.
    shr ecx, 1 ; Divide data length by 2 - calculate data length in words.
    mov edi, [ebp+24] ; Copy data loading address to edi.
    rep insw ; Copy all data there.

    mov ecx, ebx ; Copy number of sectors from ebx to ecx.
    shr ecx, 9 ; Multiply ecx by sector size.
    sub ecx, [ebp+16] ; Substract offset from ecx.
    sub ecx, [ebp+20] ; Substract data length from ecx.
                      ; Remaining data length in the last read sector is stored in ecx.
    shr ecx, 1 ; Divide remaining data length by 2 - calculate remaining data length in words.
    cmp ecx, 0 ; If remaining data length equals zero return.
    je .return

.dump_remains:
    in ax, dx ; If remaining data length doesn't equal zero, drain remaining data.
    loop .dump_remains

    mov ax, 0 ; Move zero (successful code) to eax.


.return:
    mov [esp+14], ax ; Change saved eax to current.
                     ; This allows us to return error or successful execution code.
    popa ; Restore all registers.
    mov esp, ebp ; Restore stack.
    pop ebp ; Restore stack base.
    ret
