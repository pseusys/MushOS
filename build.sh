nasm loader.asm -f bin -o loader.bin
gcc -m32 -fno-pie -c kernel.c -o kernel.o
ld -m elf_i386 -o kernel.elf -Ttext 0x1000 kernel.o
objcopy -O binary kernel.elf kernel.bin
cat loader.bin kernel.bin > image
qemu-system-i386 image
