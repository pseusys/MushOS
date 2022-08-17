# Adding kernel target
file (GLOB_RECURSE mush_core drivers/* kernel/*)
list (PREPEND mush_core "boot/kernel_gate.asm")

add_executable(MushCore_kernel ${mush_core})
target_link_options(MushCore_kernel PRIVATE -m elf_i386 -n --section-start=.got.plt=0x8000 -Tdata 0x8100 -Tbss 0x8500 -Ttext 0x9000)
target_link_libraries(MushCore_kernel LINK_PUBLIC ${MUSHLIB} ${MushOS_loader})
set_target_properties(MushCore_kernel PROPERTIES SUFFIX ".elf")
add_custom_command(TARGET MushCore_kernel POST_BUILD COMMAND ${OBJECT_COPY} -O binary $<TARGET_FILE:MushCore_kernel> $<TARGET_FILE:MushCore_kernel>.bin VERBATIM)


# Adding bootloader target
add_library (MushCore_bootloader OBJECT boot/loader.asm)
set_target_properties (MushCore_bootloader PROPERTIES NASM_OBJ_FORMAT bin)

# Merging resulting files to MushOS image file, resetting 'MUSHOS_IMG' variable
add_custom_command (OUTPUT MushCore_complete.bin COMMAND ${CONCATENATE} $<TARGET_OBJECTS:MushCore_bootloader> $<TARGET_FILE:MushCore_kernel>.bin > MushCore_complete.bin COMMAND ${TRUNCATE} -s 4194304 MushCore_complete.bin DEPENDS MushCore_kernel MushCore_bootloader)
set (MUSHOS_IMG ${CMAKE_CURRENT_BINARY_DIR}/MushCore_complete.bin PARENT_SCOPE)
add_custom_target (MushCore DEPENDS MushCore_complete.bin)
