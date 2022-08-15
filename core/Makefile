C_SOURCES = $(wildcard ./kernel/*.c ./drivers/*.c)
ASM_SOURCES = $(wildcard ./kernel/*.asm ./drivers/*.asm )
HEADERS = $(wildcard ./kernel/*.h ./drivers/*.h)
OBJECTS = $(subst ./kernel/,./build/,$(subst ./drivers/,./build/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj)))

build: create ./images/floppy.img

./images/floppy.img: ./build/loader.bin ./build/kernel.bin
	cat $^ > ./images/floppy.img
	truncate -s 4194304 ./images/floppy.img

./build/loader.bin:
	nasm ./boot/loader.asm -f bin -o $@

./build/kernel.bin: ./build/kernel.elf
	objcopy -O binary $< $@

./build/kernel.elf: ./build/kernel_gate.o $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj)
	ld -m elf_i386 -o $@ -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000 $< $(OBJECTS) -L ../lib/bin -l:mushlib.a

./build/kernel_gate.o:
	nasm ./boot/kernel_gate.asm -f elf -o $@

%.o: %.c $(HEADERS)
	gcc -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -ffreestanding -c $< -I ../lib/bin -o ./build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

create:
	if [ ! -e ./images ]; then mkdir ./images; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./images ]; then rm -r ./images; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
