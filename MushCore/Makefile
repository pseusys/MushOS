C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c ../MushLib/*.c ../MushCreator/file_system/file_input.c)
ASM_SOURCES = $(wildcard ./kernel/*.asm ./drivers/*.asm ../MushLib/*.asm)
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h ./MushLib/*.h ../MushCreator/file_system/file_input.h)
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.obj}

build: clean create ./images/floppy.img

debug: clean run

run: create ./images/floppy.img
	wc -c ./images/floppy.img;
	qemu-system-i386 -vga std -drive format=raw,file=./images/floppy.img

test: clean create ./images/floppy.img
	qemu-system-i386 -curses -drive format=raw,file=./images/floppy.img

./images/floppy.img: ./build/loader.bin ./build/kernel.bin
	cat $^ > ./images/floppy.img

./build/loader.bin:
	nasm ./boot/loader.asm -f bin -o $@

./build/kernel.bin: ./build/kernel.elf
	objcopy -O binary $< $@

./build/kernel.elf: ./build/kernel_gate.o ${OBJ}
	ld -m elf_i386 -o $@ -Tdata 0x8000 -Tbss 0x8500 -Ttext 0xa000 $< ${subst ./kernel/,./build/,$(subst ./drivers/,./build/,$(subst ../MushLib/,./build/,$(subst ../MushCreator/file_system/,./build/,$(OBJ))))}

./build/kernel_gate.o:
	nasm ./boot/kernel_gate.asm -f elf -o $@

%.o: %.c ${HEADERS}
	gcc -DMUSHENV -m32 -ffreestanding -c $< -o ./build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

create:
	if [ ! -e ./images ]; then mkdir ./images; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./images ]; then rm -r ./images; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
