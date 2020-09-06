void _start() {
    char* silos = "Beep-beep\n";
    //asm volatile ("int %0" :: "i"(49));
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
    //asm volatile ("int %0" :: "i"(49));
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
    asm volatile ("pop %eax");
}
