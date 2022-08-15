C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c ../MushLib/*.c)
ASM_SOURCES = $(wildcard ./kernel/*.asm ./drivers/*.asm ../MushLib/*.asm)
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h ./MushLib/*.h)
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.obj}

build: create ./images/floppy.img

./images/floppy.img: ./build/loader.bin ./build/kernel.bin
	cat $^ > ./images/floppy.img

./build/loader.bin:
	nasm ./boot/loader.asm -f bin -o $@

./build/kernel.bin: ./build/kernel.elf
	objcopy -O binary $< $@

./build/kernel.elf: ./build/kernel_gate.o ${OBJ}
	ld -m elf_i386 -o $@ -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000 $< ${subst ./kernel/,./build/,$(subst ./drivers/,./build/,$(subst ../MushLib/,./build/,$(OBJ)))}

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
