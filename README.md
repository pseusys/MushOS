[![BUILD](https://github.com/pseusys/MushOS/actions/workflows/build.yml/badge.svg)](https://github.com/pseusys/MushOS/actions/workflows/build.yml)
[![TEST](https://github.com/pseusys/MushOS/actions/workflows/test.yml/badge.svg)](https://github.com/pseusys/MushOS/actions/workflows/test.yml)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/4018/badge)](https://bestpractices.coreinfrastructure.org/projects/4018)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=pseusys_MushOS&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=pseusys_MushOS)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# MushOS

MushOS is a UNIX-like OS prototype, written from scratch.  
My motivation for creating it was:
1. Research how computer works - on the deepest level of programming, where software meets hardware.
2. Trying to resolve some low-level issues I've always been wondering about (e.g.: inability to receive allocated memory amount from pointer).

Kernel code is stored in `core` directory, `lib` directory contains code for `mushlib` - an stdlib replacement for MushOS.  


### Roadmap

1. Standalone bootloader 👍
2. Kernel code 👍
3. Keyboard + screen adapter 👍
4. MushLib basics 👍
5. Limitations, checks and exceptions 🤙
6. Kernel memory map 👎
7. File system (MushFS) + initrd 👎
8. Shell + audio drivers 👎
9. Multitasking 👎
10. User mode 👎
11. Video mode 👎
12. Sample apps 👎
13. Create cross-compilers instead of using standard with flags 👎
14. MushTest with asserts, etc. 👎
15. CMakeLists: variable single case, single system of notifying of missing utilities. 👎
16. Docs: custom groups with custom layouts instead of the `usergroup`'s. 👎
17. etc...


### Build and run

Build MushLib using following command (cmake package required!):  
`make build_lib`

Run MushOS using following command (cmake and qemu packages required!):  
`make run`


### Library modules

MushLib is considered to be a modular entity - there are no limitations on *how* heap or filesystem is implemented as long as the implementation has the functions matching standard signatures (from `lib/base`).

For every build of MushLib it's possible to replace its sources with custom ones.
For example, if you'd like to build MushLib with custom heap implementation, you just have to create a *.c* file that has the functions defined in `lib/base/heap.h` - and then just embed it into MushLib using the following command (the same works with *.h* and *.asm* sources):  
`make C_SOURCES="path/to/your/heap.c;" build_lib`

> **_NB!_** Your file has to have the same name as base MushLib file for successful replacement.
> If two different files need to be replaced, just use a semicolon (one semicolon in the end is always required!).

### Requirements

#### Test:
1. `gcc-multilib`
