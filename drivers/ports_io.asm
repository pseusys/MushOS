[bits 32]
global low_port_byte_in
global low_port_byte_out
global low_port_word_in
global low_port_word_out

low_port_byte_in:
    push ebp
    mov ebp, esp

    push edx
    mov dx, [ebp+8]
    in al, dx
    pop edx

    mov esp, ebp
    pop ebp
    ret

low_port_byte_out:
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

low_port_word_in:
    push ebp
    mov ebp, esp

    push edx
    mov dx, [ebp+8]
    in ax, dx
    pop edx

    mov esp, ebp
    pop ebp
    ret

low_port_word_out:
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

