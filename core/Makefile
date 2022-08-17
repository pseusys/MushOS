C_SOURCES = $(wildcard /home/milty/Documents/MushOS/core/kernel/*.c /home/milty/Documents/MushOS/core/drivers/*.c)
ASM_SOURCES = $(wildcard /home/milty/Documents/MushOS/core/kernel/*.asm /home/milty/Documents/MushOS/core/drivers/*.asm )
HEADERS = $(wildcard /home/milty/Documents/MushOS/core/kernel/*.h /home/milty/Documents/MushOS/core/drivers/*.h)
OBJECTS = $(subst /home/milty/Documents/MushOS/core/kernel/,/home/milty/Documents/MushOS/core/build/,$(subst /home/milty/Documents/MushOS/core/drivers/,/home/milty/Documents/MushOS/core/build/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj)))

build: create /home/milty/Documents/MushOS/core/images/floppy.img

/home/milty/Documents/MushOS/core/images/floppy.img: /home/milty/Documents/MushOS/core/build/loader.bin /home/milty/Documents/MushOS/core/build/kernel.bin
	cat $^ > /home/milty/Documents/MushOS/core/images/floppy.img
	truncate -s 4194304 /home/milty/Documents/MushOS/core/images/floppy.img

/home/milty/Documents/MushOS/core/build/loader.bin:
	nasm /home/milty/Documents/MushOS/core/boot/loader.asm -f bin -o $@

/home/milty/Documents/MushOS/core/build/kernel.bin: /home/milty/Documents/MushOS/core/build/kernel.elf
	objcopy -O binary $< $@

/home/milty/Documents/MushOS/core/build/kernel.elf: /home/milty/Documents/MushOS/core/build/kernel_gate.o $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj)
	ld -m elf_i386 -o $@ -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000 $< $(OBJECTS) -L ../lib/bin -l:mushlib.a

/home/milty/Documents/MushOS/core/build/kernel_gate.o:
	nasm /home/milty/Documents/MushOS/core/boot/kernel_gate.asm -f elf -o $@

%.o: %.c $(HEADERS)
	cc -g -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -L -ffreestanding -std=gnu11 -c $< -I /home/milty/Documents/MushOS/lib/bin -o /home/milty/Documents/MushOS/core/build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

create:
	if [ ! -e ./images ]; then mkdir ./images; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./images ]; then rm -r ./images; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
