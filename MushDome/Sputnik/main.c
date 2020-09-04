void _start() {
    asm volatile ("push %0" :: "r"(1));
    asm volatile ("push %0" :: "r"('U'));
    asm volatile ("push %0" :: "r"(15u));
    asm volatile ("push %0" :: "r"(0u));
    asm volatile (
        "push %%ebp\n"
        "mov %%esp, %%ebp\n"
        "int %0\n"
        "mov %%ebp, %%esp\n"
        "pop %%ebp" :: "i"(48)
    );
}
