.DEFAULT_GOAL=run

VERSIONING_FILES = CMakeLists.txt Makefile Doxyfile
CURRENT_VERSION = 0.1

ASM_SOURCES = ""
C_SOURCES = ""
H_SOURCES = ""

VERSION_INCREMENT = "patch"

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
	cmake -S ./ -B build -D ASM_SOURCES=$(ASM_SOURCES) -D C_SOURCES=$(C_SOURCES) -D H_SOURCES=$(H_SOURCES)
.PHONY: prepare_build

build_lib: prepare_build
	cmake --build build --target MushLib --
.PHONY: build_lib

build_img: prepare_build venv
	cmake --build build --target MushCore --
.PHONY: build_img


test: prepare_build venv
	cmake --build build --target MushTest --
	cd build/tests && ctest --verbose && cd ../..
.PHONY: test

docs:
	rm -rf docs/assets/*.png
	drawio -x -f png -o docs/assets/ docs/assets/
	doxygen docs/Doxyfile
.PHONY: docs


run: build_img
	qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=build/artifacts/MushOS.img
.PHONY: run

rerun: clean run
.PHONY: rerun


version: venv
	bump2version --current-version $(CURRENT_VERSION) $(VERSION_INCREMENT) $(VERSIONING_FILES)
.PHONY: version


clean:
	rm -rf build
	rm -rf docs/html
	rm -rf docs/assets/*.png
	rm -rf cmake-build-*
	rm -rf **/__pycache__
.PHONY: clean

clean_all: clean
	rm -rf venv
.PHONY: clean_all
