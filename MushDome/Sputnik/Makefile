C_SOURCES = $(wildcard ./code/main.c )
HEADERS = $(wildcard )
OBJ = ${C_SOURCES:.c=.o}

build: clean create ${OBJ}
	ld -m elf_i386 -o ./build/main.elf -n --section-start=.got.plt=0x104000 -Tdata 0x104100 -Tbss 0x104200 -Ttext 0x104300 ${subst ../../MushLib/,./build/,$(subst ./code/,./build/,$(OBJ))}

%.o: %.c ${HEADERS}
	gcc -DMUSHENV -m32 -ffreestanding -c $< -o ./build/$(@F)

create:
	if [ ! -e ./build ]; then mkdir ./build; fi;

clean:
	if [ -e ./build ]; then rm -r ./build; fi;