C_SOURCES = $(wildcard ./adapter/*.c ./file_system/*.c ./utils/*.c main.c)
HEADERS = $(wildcard ./adapter/*.h ./file_system/*.h ./utils/*.h)
OBJ = ${C_SOURCES:.c=.o}

debug: clean run

run: create build
	./fs/creator.elf

build: %.c ${HEADERS}
	gcc $< -o ./fs/creator.elf

create:
	if [ ! -e ./fs ]; then mkdir ./fs; fi;

clean:
	if [ -e ./fs ]; then rm -r ./fs; fi;