.DEFAULT_GOAL=all

VERSIONING_FILES = CMakeLists.txt
CURRENT_VERSION = 0.1

help:
	@echo
.PHONY: help


prepare_build:
	cmake -S ./ -B build -G "Unix Makefiles"
.PHONY: prepare_build

build_lib: prepare_build
	cmake --build build --target MushLib --
.PHONY: build_lib

build_img: prepare_build
	cmake --build build --target MushCore --
.PHONY: build_img

build_all: prepare_build
	cmake --build build --target all --
.PHONY: build_all


run:
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=build/artifacts/MushOS.img
.PHONY: run


all: build_all run
.PHONY: all


clean:
	rm -rf ./build
	rm -rf ./cmake-build-*
.PHONY: clean
