C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c)
ASM_SOURCES = $(wildcard ./kernel/*.asm ./drivers/*.asm)
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h)
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.obj}

run: all
	qemu-system-i386 -vga std -drive format=raw,file=./images/floppy.img

all: create ./images/floppy.img

create:
	echo ${OBJ}

./images/floppy.img: ./build/loader.bin ./build/kernel.bin
	cat $^ > ./images/floppy.img

./build/loader.bin:
	nasm ./boot/loader.asm -f bin -o $@

./build/kernel.bin: ./build/kernel.elf
	objcopy -O binary $< $@

./build/kernel.elf: ./build/kernel_gate.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $< ${subst ./kernel/,./build/,$(subst ./drivers/,./build/,$(OBJ))}

./build/kernel_gate.o:
	nasm ./boot/kernel_gate.asm -f elf -o $@

%.o: %.c ${HEADERS}
	gcc -m32 -ffreestanding -c $< -o ./build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

clean:
	rm ./build/*; rm ./images/*


