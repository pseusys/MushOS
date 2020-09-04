all:
	nasm code.asm -f elf -o code.elf

build:
	gcc -DMUSHENV -m32 -ffreestanding -c main.c -o main.o
	ld -m elf_i386 -o main.elf -n --section-start=.got.plt=0x10000 -Tdata 0x10010 -Tbss 10010 -Ttext 0x10100 main.o