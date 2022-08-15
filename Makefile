clean:
	cd ./lib/; make clean;
	cd ./core/; make clean;

build_debug:
	cd ./lib/; make build;
	cd ./core/; make build;

all: clean build_debug
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=./core/images/floppy.img;
