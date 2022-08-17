C_SOURCES = $(wildcard /home/milty/Documents/MushOS/lib/code/*.c)
ASM_SOURCES = $(wildcard /home/milty/Documents/MushOS/lib/code/*.asm)
HEADERS = /home/milty/Documents/MushOS/lib/code/generic.h $(filter-out /home/milty/Documents/MushOS/lib/code/generic.h,$(wildcard /home/milty/Documents/MushOS/lib/code/*.h))
OBJECTS = $(subst /home/milty/Documents/MushOS/lib/code/,/home/milty/Documents/MushOS/lib/build/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj))

build: create /home/milty/Documents/MushOS/lib/bin/mushlib.h $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj) /home/milty/Documents/MushOS/lib/bin/mushlib.a

/home/milty/Documents/MushOS/lib/bin/mushlib.a:
	ar cr $@ $(OBJECTS)

/home/milty/Documents/MushOS/lib/bin/mushlib.h:
	cat $(HEADERS) | sed '/#include\|#ifndef\|#define MUSHLIB\|#endif/d' > /home/milty/Documents/MushOS/lib/bin/proto_mushlib
	echo "#ifndef MUSHLIB\n#define MUSHLIB\n" > $@; cat /home/milty/Documents/MushOS/lib/bin/proto_mushlib >> $@; echo "\n#endif // MUSHLIB\n" >> $@;
	rm /home/milty/Documents/MushOS/lib/bin/proto_mushlib

%.o: %.c $(HEADERS)
	cc -g -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -ffreestanding -std=gnu11 -c $< -o /home/milty/Documents/MushOS/lib/build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o /home/milty/Documents/MushOS/lib/build/$(@F)

create:
	if [ ! -e ./bin ]; then mkdir ./bin; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./bin ]; then rm -r ./bin; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
