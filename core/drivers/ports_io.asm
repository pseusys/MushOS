[bits 32] ; Set of functions that write certain numbers into system registers.
global port_byte_in ; Read byte into register.
global port_byte_out ; Write byte from register.
global port_word_in ; Read word into register.
global port_word_out ; Write word from register.

port_byte_in: ; Only parameter (word): [ESP + 8] - reading port number.
    push ebp ; Saving register base.
    mov ebp, esp ; Setting register base to register top.
                 ; Function won't be able to interfere with previous stack entries.

    push edx ; Saving EDX register.
    mov dx, [ebp+8] ; Moving register number into DX.
    in al, dx ; Reading byte from register DX to AL.
    pop edx ; Return value is contained in EAX register.

    mov esp, ebp ; Restoring stack.
    pop ebp
    ret

port_byte_out: ; First parameter (word): [ESP + 8] - reading port number.
               ; Last parameter (byte): [ESP + 12] - new register value.
    push ebp
    mov ebp, esp

    push edx
    push eax ; Saving EAX.
    mov dx, [ebp+8] ; Moving register number into DX.
    mov al, [ebp+12] ; Moving new register value into AL.
    out dx, al ; Writing byte to register DX from AL.
    pop eax
    pop edx

    mov esp, ebp
    pop ebp
    ret

port_word_in: ; Last parameter (word): [ESP + 8] - reading port number.
    push ebp
    mov ebp, esp

    push edx
    mov dx, [ebp+8]
    in ax, dx ; Reading word from register DX to AX.
    pop edx

    mov esp, ebp
    pop ebp
    ret

port_word_out: ; First parameter (word): [ESP + 8] - reading port number.
               ; Last parameter (word): [ESP + 12] - new register value.
    push ebp
    mov ebp, esp

    push edx
    push eax
    mov dx, [ebp+8]
    mov ax, [ebp+12] ; Moving new register value into AX.
    out dx, ax ; Writing byte to register DX from AX.
    pop eax
    pop edx

    mov esp, ebp
    pop ebp
    ret
