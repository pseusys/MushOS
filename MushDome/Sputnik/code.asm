main:
    push ebp
    mov ebp, esp
    pusha

    push 0
    push 15
    push 'U'
    push 1

    push ebp
    mov ebp, esp
    int 48
    mov esp, ebp
    pop ebp

    popa
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret