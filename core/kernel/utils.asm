[bits 32]
global copy_page_physical
global get_instruction_pointer

copy_page_physical:
   push ebp              ; Save stack base.
   mov ebp, esp          ; Set register base to register top.
   pusha                 ; According to __cdecl, we must preserve the contents of EBX.
   pushf                 ; push EFLAGS, so we can pop it and reenable interrupts
                         ; later, if they were enabled anyway.
   cli                   ; Disable interrupts, so we aren't interrupted.
                         ; Load these in BEFORE we disable paging!
   mov ebx, [ebp+8]      ; Source address
   mov ecx, [ebp+12]     ; Destination address

   mov edx, cr0          ; Get the control register...
   and edx, 0x7fffffff   ; and...
   mov cr0, edx          ; Disable paging.

   mov edx, 1024         ; 1024*4bytes = 4096 bytes to copy

.loop:
   mov eax, [ebx]        ; Get the word at the source address
   mov [ecx], eax        ; Store it at the dest address
   add ebx, 4            ; Source address += sizeof(word)
   add ecx, 4            ; Dest address += sizeof(word)
   dec edx               ; One less word to do
   jnz .loop

   mov edx, cr0          ; Get the control register again
   or  edx, 0x80000000   ; and...
   mov cr0, edx          ; Enable paging.

   popf                  ; Pop EFLAGS back.
   popa                  ; Get the original value of EBX back.
   mov esp, ebp          ; Restore stack.
   pop ebp               ; Restore stack base.
   ret


get_instruction_pointer:
  pop eax
  jmp eax
