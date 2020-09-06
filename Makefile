all:
	cd ./MushCore/; make build;
	cd ./MushDome/Sputnik; make build;
	cd ./MushCreator/; make debug;
	qemu-system-i386 -d guest_errors -vga std -drive format=raw,file=./MushCreator/fs/FS.binary;

build_debug:
	cd ./MushCore/; make build;
	cd ./MushCreator/; make debug;
