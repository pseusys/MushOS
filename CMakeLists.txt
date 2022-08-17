cmake_minimum_required(VERSION 3.16)
# TODO: add version to artifacts
project(MushOS LANGUAGES C ASM_NASM VERSION 0.1)

set(CMAKE_C_STANDARD 11)

# Special variables used in building process:
# OBJECT_COPY - should be path to `objcopy` utility
if (NOT DEFINED OBJECT_COPY)
    set(OBJECT_COPY objcopy)
endif ()
# OBJECT_LINKER - should be path to `ld` linker
if (NOT DEFINED OBJECT_LINKER)
    set(OBJECT_LINKER ld)
endif()
# CONCATENATE - should be path to `cat` utility
if (NOT DEFINED CONCATENATE)
    set(CONCATENATE cat)
endif()
# TRUNCATE - should be path to `truncate` utility
if (NOT DEFINED TRUNCATE)
    set(TRUNCATE truncate)
endif()

# Configuring ASM build process: default output format, compilation and linking (using standard linker)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${OBJECT_LINKER} <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Configuring ASM build process: linking is done using standard linker
set(CMAKE_C_LINK_EXECUTABLE "${OBJECT_LINKER} <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Adding compile options: cross-compile for C files and setting output format for ASM files
add_compile_options("$<$<COMPILE_LANG_AND_ID:C,GNU>:-nostartfiles;-nodefaultlibs;-nolibc;-nostdlib;-DMUSHENV;-m32;-ffreestanding>")
add_compile_options("$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>,$<TARGET_PROPERTY:NASM_OBJ_FORMAT>,${CMAKE_ASM_NASM_OBJECT_FORMAT}>>")


# Adding 'MushLib' path, setting directory it will be built in
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set (MUSHLIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")

# Including directory and finding library
include_directories(${MUSHLIB_DIRECTORY})
find_library(MUSHLIB MushLib PATHS ${MUSHLIB_DIRECTORY})


# Adding kernel target TODO: move to separate dir
file(GLOB_RECURSE mush_core core/drivers/* core/kernel/*)
list (PREPEND mush_core "core/boot/kernel_gate.asm")

# TODO: clion set executable, this executable is false
add_executable(MushOS_kernel ${mush_core})
target_link_options(MushOS_kernel PRIVATE -m elf_i386 -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000)
set_target_properties(MushOS_kernel PROPERTIES SUFFIX ".elf")
target_link_libraries(MushOS_kernel LINK_PUBLIC ${MUSHLIB} ${MushOS_loader})
add_custom_command(TARGET MushOS_kernel POST_BUILD COMMAND ${OBJECT_COPY} -O binary $<TARGET_FILE:MushOS_kernel> $<TARGET_FILE:MushOS_kernel>.bin VERBATIM)


# Adding bootloader target TODO: move to separate dir
add_library(MushOS_loader OBJECT core/boot/loader.asm)
set_target_properties(MushOS_loader PROPERTIES NASM_OBJ_FORMAT bin)


# Merging resulting files to MushOS image
add_custom_command(OUTPUT MushOS_complete COMMAND ${CONCATENATE} $<TARGET_OBJECTS:MushOS_loader> $<TARGET_FILE:MushOS_kernel>.bin > MushOS_complete COMMAND ${TRUNCATE} -s 4194304 MushOS_complete DEPENDS MushOS_kernel MushOS_loader)

# Execute debug command TODO: fix, not running
execute_process(COMMAND "qemu-system-x86_64 -d guest_errors -vga std -drive format=raw,file=$<TARGET_FILE:MushOS_complete>" VERBATIM)

# Adding final target for building all MushOS parts TODO: fix if needed
add_custom_target(run ALL DEPENDS MushOS_complete)
