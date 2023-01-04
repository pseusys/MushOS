set (KERNEL_MAX_GOT_PLT 100)
set (KERNEL_MAX_DATA 400)
set (KERNEL_MAX_BSS 500)
set (KERNEL_MAX_TEXT A000)

set (KERNEL_START_GOT_PLT 8000)
set (KERNEL_START_DATA 8100)
set (KERNEL_START_BSS 8500)
set (KERNEL_START_TEXT 9000)


# Find sources for kernel
file (GLOB_RECURSE mush_core drivers/* kernel/*)
list (PREPEND mush_core "boot/kernel_gate.asm")

# Add kernel target
add_executable (MushCore_kernel ${mush_core})
target_link_options (MushCore_kernel PRIVATE -m elf_i386 -n --section-start=.got.plt=0x${KERNEL_START_GOT_PLT} -Tdata 0x${KERNEL_START_DATA} -Tbss 0x${KERNEL_START_BSS} -Ttext 0x${KERNEL_START_TEXT})
target_link_libraries (MushCore_kernel LINK_PUBLIC MushLib)
set_target_properties (MushCore_kernel PROPERTIES SUFFIX ".elf")
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${OBJECT_COPY} -O binary $<TARGET_FILE:MushCore_kernel> $<TARGET_FILE:MushCore_kernel>.bin VERBATIM)

# add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${CMAKE_COMMAND} -D IN_FILE=$<TARGET_FILE:MushCore_kernel> -D PATTERN="/^\.got\.plt/" -D MAX_VALUE=${KERNEL_MAX_GOT_PLT} -P validate_kernel.cmake)

# Add bootloader target
add_library (MushCore_bootloader OBJECT boot/loader.asm)
set_target_properties (MushCore_bootloader PROPERTIES NASM_OBJ_FORMAT bin)

# Merge resulting files to MushOS image file, setting 'MUSHOS_IMG' variable
add_custom_command (OUTPUT MushCore_complete.bin COMMAND ${CONCATENATE} $<TARGET_OBJECTS:MushCore_bootloader> $<TARGET_FILE:MushCore_kernel>.bin > MushCore_complete.bin COMMAND ${TRUNCATE} -s 4194304 MushCore_complete.bin DEPENDS MushCore_kernel MushCore_bootloader)
set (MUSHOS_IMG ${CMAKE_CURRENT_BINARY_DIR}/MushCore_complete.bin PARENT_SCOPE)
add_custom_target (MushCore DEPENDS MushCore_complete.bin)
