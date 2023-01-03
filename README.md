[![Build Status](https://travis-ci.org/MilesArtemius/MushOS.svg?branch=master)](https://travis-ci.org/MilesArtemius/MushOS)
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
6. Kernel memory map 👎
7. File system (MushFS) + initrd 👎
8. Shell + audio drivers 👎
9. Multitasking 👎
10. User mode 👎
11. Video mode 👎
12. Sample apps 👎
13. etc...

Build using following command (cmake package required!):  
`make build_all`

Run using following command (qemu package required!):  
`make run`
