[![BUILD](https://github.com/pseusys/MushOS/actions/workflows/build.yml/badge.svg)](https://github.com/pseusys/MushOS/actions/workflows/build.yml)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/4018/badge)](https://bestpractices.coreinfrastructure.org/projects/4018)
[![Documentation Status](https://readthedocs.org/projects/mushos/badge/?version=latest)](https://mushos.readthedocs.io/en/latest/?badge=latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# MushOS

MushOS is a UNIX-like OS prototype, written from scratch.  
My motivation for creating it was:
1. Research how computer works - on the deepest level of programming, where software meets hardware.
2. Trying to resolve some low-level issues I've always been wondering about (e.g.: inability to receive allocated memory amount from pointer).

Kernel code is stored in `core` directory, `lib` directory contains code for `mushlib` - an stdlib replacement for MushOS.  
MushOS roadmap:
1. Standalone bootloader 👍
2. Kernel code 👍
3. Keyboard + screen adapter 👍
4. MushLib basics 👍
5. GRUB loader booting 🤙
6. Limitations, checks and exceptions 🤙
7. Kernel memory map 👎
8. File system (MushFS) + initrd 👎
9. Shell + audio drivers 👎
10. Multitasking 👎
11. User mode 👎
12. Video mode 👎
13. Sample apps 👎
14. etc...

Build using following command (cmake package required!):  
`make build_all`

Run using following command (qemu package required!):  
`make run`


### Library modules

MushLib is considered to be a modular entity - there are no limitations on *how* heap or filesystem is implemented as long as the implementation has the functions matching standard signatures (from `lib/base`).

For every build of MushLib it's possible to add and replace its sources.
For example if you'd like to build MushLib with custom heap implementation, you just have to create a *.c* file pair that has the functions defined in `lib/base/heap` - and then just embed it into MushLib using the following command:
`make REPLACE_SOURCES="path/to/your/heap.c;" run`

> **_NB!_** Your file has to have the same name as base mushlib file for successful replacement.
> **_NB!_** If two different files need to be replaced, just use a semicolon (one semicolon in the end is always required!).
