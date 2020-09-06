all:
	nasm code.asm -f elf -o code.elf

build:
	gcc -DMUSHENV -m32 -ffreestanding -c main.c -o main.o
	ld -m elf_i386 -o main.elf -n --section-start=.got.plt=0x100000 -Tdata 0x100010 -Tbss 0x100010 -Ttext 0x100100 main.o