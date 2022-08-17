cmake_minimum_required(VERSION 3.16)
project(MushOS LANGUAGES C ASM_NASM VERSION 0.1)

set(CMAKE_C_STANDARD 11)

# Special variables used in build:
# CMAKE_ASM_NASM_LINKER - should be path to `ld` linker
# OBJECT_COPY - should be path to `objcopy` utility
set(OBJECT_COPY objcopy) # TODO: replace with search, add other console commands
set(CMAKE_LINKER ld)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "<CMAKE_ASM_NASM_LINKER> <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_compile_options("$<$<COMPILE_LANG_AND_ID:C,GNU>:-nostartfiles;-nodefaultlibs;-nolibc;-nostdlib;-DMUSHENV;-m32;-ffreestanding>")
add_compile_options("$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>,$<TARGET_PROPERTY:NASM_OBJ_FORMAT>,${CMAKE_ASM_NASM_OBJECT_FORMAT}>>")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set (MUSHLIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")

include_directories(${MUSHLIB_DIRECTORY})
find_library(MUSHLIB mushlib PATHS ${MUSHLIB_DIRECTORY})


file(GLOB_RECURSE mush_core core/drivers/* core/kernel/*)
list (PREPEND mush_core "core/boot/kernel_gate.asm")

add_executable(MushOS_kernel ${mush_core})
target_link_options(MushOS_kernel PRIVATE -m elf_i386 -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000)
set_target_properties(MushOS_kernel PROPERTIES SUFFIX ".elf")
target_link_libraries(MushOS_kernel LINK_PUBLIC ${MUSHLIB} ${MushOS_loader})
add_custom_command(TARGET MushOS_kernel POST_BUILD COMMAND objcopy -O binary $<TARGET_FILE:MushOS_kernel> $<TARGET_FILE:MushOS_kernel>.bin VERBATIM)

add_library(MushOS_loader OBJECT core/boot/loader.asm)
set_target_properties(MushOS_loader PROPERTIES NASM_OBJ_FORMAT bin PREFIX "" SUFFIX ".bin")


add_custom_command(OUTPUT MushOS_complete COMMAND cat $<TARGET_OBJECTS:MushOS_loader> $<TARGET_FILE:MushOS_kernel> > MushOS_complete COMMAND truncate -s 4194304 MushOS_complete DEPENDS MushOS_kernel MushOS_loader)
execute_process(COMMAND "qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=$<TARGET_FILE:MushOS_complete>" VERBATIM)

add_custom_target(run ALL DEPENDS MushOS_complete)
