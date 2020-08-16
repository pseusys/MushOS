C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c ./mushlib/*.c)
ASM_SOURCES = $(wildcard ./kernel/*.asm ./drivers/*.asm ./mushlib/*.asm)
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h ./mushlib/*.h)
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
	du -b ./build/kernel.bin > ./build/kernel_size.sot #Read segments and write into bootloader.

./build/kernel.elf: ./build/kernel_gate.o ${OBJ} ./build/kernel_follower.o
	ld -m elf_i386 -o $@ -Tdata 0x8000 -Tbss 0x9000 -Ttext 0xa000 --defsym=sentinel=0x10000 $< ${subst ./kernel/,./build/,$(subst ./drivers/,./build/,$(subst ./mushlib/,./build/,$(OBJ)))} ./build/kernel_follower.o

./build/kernel_gate.o:
	nasm ./boot/kernel_gate.asm -f elf -o $@

./build/kernel_follower.o:
	nasm ./boot/kernel_follower.asm -f elf -o $@

%.o: %.c ${HEADERS}
	gcc -m32 -ffreestanding -c $< -o ./build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

create:
	if [ ! -e ./images ]; then mkdir ./images; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./images ]; then rm -r ./images; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
