all:
	cd ./MushCore/; make build;
	cd ./MushCreator/; make debug;
	qemu-system-i386 -vga std -drive format=raw,file=./MushCreator/fs/FS.binary;

build_debug:
	cd ./MushCore/; make build;
	cd ./MushCreator/; make debug;
