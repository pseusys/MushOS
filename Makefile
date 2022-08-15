clean:
	cd ./MushCore/; make clean;

build_debug:
	cd ./MushCore/; make build;

rerun: clean build_debug
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=./MushCore/images/floppy.img;
