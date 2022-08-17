clean:
	cd ./lib/; make clean;
	cd ./core/; make clean;

all: clean
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=/home/milty/Documents/MushOS/cmake-build-debug/MushOS_complete;
