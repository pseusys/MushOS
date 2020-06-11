[bits 32]
global port_byte_in
global port_byte_out
global port_word_in
global port_word_out

port_byte_in:
    push ebp
    mov ebp, esp

    push edx
    mov dx, [ebp+8]
    in al, dx
    pop edx

    mov esp, ebp
    pop ebp
    ret

port_byte_out:
    push ebp
    mov ebp, esp

    push edx
    push eax
    mov dx, [ebp+8]
    mov al, [ebp+12]
    out dx, al
    pop eax
    pop edx

    mov esp, ebp
    pop ebp
    ret

port_word_in:
    push ebp
    mov ebp, esp

    push edx
    mov dx, [ebp+8]
    in ax, dx
    pop edx

    mov esp, ebp
    pop ebp
    ret

port_word_out:
    push ebp
    mov ebp, esp

    push edx
    push eax
    mov dx, [ebp+8]
    mov ax, [ebp+12]
    out dx, ax
    pop eax
    pop edx

    mov esp, ebp
    pop ebp
    ret
