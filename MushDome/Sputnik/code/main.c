#define orbital_slot 0x100000


void _start() {
    char* silos = "Beep-beep\n";

    //initialize_heap((void*) orbital_slot, 0x3000);
    //warn("Sputnik says: %s\n", silos)


    asm volatile ("push %0" :: "r"(0));
    asm volatile ("push %0" :: "r"(0));
    asm volatile ("push %0" :: "r"(15));
    asm volatile ("push %0" :: "r"(silos));
    asm volatile ("push %0" :: "r"(2));
    asm volatile (
        "push %%ebp\n"
        "mov %%esp, %%ebp\n"
        "int %0\n"
        "mov %%ebp, %%esp\n"
        "pop %%ebp" :: "i"(48)
    );
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");

    //asm volatile ("int %0" :: "i"(49));
    //asm volatile ("jmp .");
}
