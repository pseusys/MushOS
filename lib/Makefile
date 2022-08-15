C_SOURCES = $(wildcard ./code/*.c)
ASM_SOURCES = $(wildcard ./code/*.asm)
HEADERS = ./code/generic.h $(filter-out ./code/generic.h,$(wildcard ./code/*.h))
OBJECTS = $(subst ./code/,./build/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj))

build: create ./bin/mushlib.h $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.obj) ./bin/mushlib.a

./bin/mushlib.a:
	ar cr $@ $(OBJECTS)

./bin/mushlib.h:
	cat $(HEADERS) | sed '/#include\|#ifndef\|#define MUSHLIB\|#endif/d' > ./bin/proto_mushlib
	echo "#ifndef MUSHLIB\n#define MUSHLIB\n" > $@; cat ./bin/proto_mushlib >> $@; echo "\n#endif // MUSHLIB\n" >> $@;
	rm ./bin/proto_mushlib

%.o: %.c $(HEADERS)
	gcc -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -L -ffreestanding -c $< -o ./build/$(@F)

%.obj: %.asm
	nasm $< -f elf -o ./build/$(@F)

create:
	if [ ! -e ./bin ]; then mkdir ./bin; fi;
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./bin ]; then rm -r ./bin; fi;
	if [ -e ./build ]; then rm -r ./build; fi;
