C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c)
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h)
OBJ = ${C_SOURCES:.c=.o}

run: all
	qemu-system-i386 ./images/floppy.img

all: create ./images/floppy.img collect

create:
	echo ${OBJ}

./images/floppy.img: ./build/loader.bin ./build/kernel.bin
	cat $^ > ./images/floppy.img

./build/loader.bin:
	nasm ./boot/loader.asm -f bin -o $@

./build/kernel.bin: ./build/kernel.elf
	objcopy -O binary $< $@

./build/kernel.elf: ./build/kernel_gate.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^

./build/kernel_gate.o:
	nasm ./boot/kernel_gate.asm -f elf -o $@

%.o: %.c ${HEADERS}
	cc -m32 -ffreestanding -c $^ -o $@

collect:
	find -name "*.o" -exec mv -i {} -t ./build \;

clean:
	rm ./build/*; rm ./images/*

