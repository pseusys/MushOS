.DEFAULT_GOAL=run

VERSIONING_FILES = CMakeLists.txt Makefile
CURRENT_VERSION = 0.1

REPLACE_SOURCES = ""
ADD_SOURCES = ""

PATH := venv/bin:$(PATH)


help:
	@echo
.PHONY: help

venv:
	@echo "Start creating virtual environment"
	python3 -m venv venv
	pip install --upgrade pip
	pip install -r scripts/requirements.txt


prepare_build:
	cmake -S ./ -B build -D REPLACE_SOURCES=$(REPLACE_SOURCES) -D ADD_SOURCES=$(ADD_SOURCES)
.PHONY: prepare_build

build_lib: prepare_build
	cmake --build build --target MushLib --
.PHONY: build_lib

build_img: prepare_build venv
	cmake --build build --target MushCore --
.PHONY: build_img


run: build_img
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=build/artifacts/MushOS.img
.PHONY: run

rerun: clean run
.PHONY: rerun


clean:
	rm -rf build
	rm -rf cmake-build-*
.PHONY: clean

clean_all: clean
	rm -rf venv
.PHONY: clean_all
