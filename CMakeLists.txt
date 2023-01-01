cmake_minimum_required (VERSION 3.20)
# TODO: add version to artifacts
project (MushOS LANGUAGES C ASM_NASM VERSION 0.1)

set (CMAKE_C_STANDARD 11)

# Special variables used in building process:
# TODO: add all paths to .gitignore
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
set (CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set (CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set (CMAKE_ASM_NASM_LINK_EXECUTABLE "${OBJECT_LINKER} <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Configuring ASM build process: linking is done using standard linker
set (CMAKE_C_LINK_EXECUTABLE "${OBJECT_LINKER} <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Adding compile options: cross-compile for C files and setting output format for ASM files
add_compile_options ("$<$<COMPILE_LANG_AND_ID:C,GNU>:-nostartfiles;-nodefaultlibs;-nolibc;-nostdlib;-DMUSHENV;-m32;-ffreestanding>")
add_compile_options ("$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>,$<TARGET_PROPERTY:NASM_OBJ_FORMAT>,${CMAKE_ASM_NASM_OBJECT_FORMAT}>>")


# Adding 'MushLib' path, setting directory it will be built in
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set (MUSHLIB_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")

# Including directory and finding library
include_directories (${MUSHLIB_DIRECTORY})
find_library (MUSHLIB mushlib PATHS ${MUSHLIB_DIRECTORY})


# Setting variable that will contain final output image file (for now to NO, it will be reset in subdirs)
set (MUSHOS_IMG NO)

# Including directories forming MushOS image, one of them will set 'MushOS' variable
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/core/)


# Copying system image file to top level directory and adding final target for building all MushOS parts
add_custom_target (run ALL DEPENDS MushLib MushCore)
add_custom_command (TARGET run POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${MUSHOS_IMG}" "${CMAKE_BINARY_DIR}/MushOS.img")
