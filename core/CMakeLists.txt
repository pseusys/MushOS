# Define maximum sizes for all sections present in kernel ELF file
set (KERNEL_MAX_GOT_PLT 100)
set (KERNEL_MAX_DATA 400)
set (KERNEL_MAX_BSS 500)
set (KERNEL_MAX_RODATA 500)
set (KERNEL_MAX_TEXT 8000)

# Define virtual addresses for all sections present in kernel ELF file
set (KERNEL_START_GOT_PLT 8000)
set (KERNEL_START_DATA 8100)
set (KERNEL_START_BSS 8500)
set (KERNEL_START_RODATA 8A00)
set (KERNEL_START_TEXT A000)


# Find sources for kernel
file (GLOB_RECURSE mush_core drivers/* kernel/* ../lib/base/*)
list (PREPEND mush_core "boot/kernel_gate.asm")

# Create headers list, 'generic.h' header should always come first to be first in library header
file (GLOB GENERIC_HEADER base/generic.h)
file (GLOB_RECURSE HEADERS base/*.h)
list (FILTER HEADERS EXCLUDE REGEX "base/generic\\.h$")
list (PREPEND HEADERS ${GENERIC_HEADER})

# Add kernel target
add_executable (MushCore_kernel ${mush_core})
target_link_options (MushCore_kernel PRIVATE -m elf_i386 -n --section-start=.got.plt=0x${KERNEL_START_GOT_PLT} -Tdata 0x${KERNEL_START_DATA} -Tbss 0x${KERNEL_START_BSS} --section-start=.rodata=${KERNEL_START_RODATA} -Ttext 0x${KERNEL_START_TEXT})
set_target_properties (MushCore_kernel PROPERTIES SUFFIX ".elf")
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${OBJECT_COPY} -O binary $<TARGET_FILE:MushCore_kernel> $<TARGET_FILE:MushCore_kernel>.bin VERBATIM)

# Check all sections present in kernel ELF file using special python script
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/check_kernel_sectors.py ".got.plt" ${KERNEL_START_GOT_PLT} ${KERNEL_MAX_GOT_PLT} $<TARGET_FILE:MushCore_kernel>)
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/check_kernel_sectors.py ".data" ${KERNEL_START_DATA} ${KERNEL_MAX_DATA} $<TARGET_FILE:MushCore_kernel>)
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/check_kernel_sectors.py ".bss" ${KERNEL_START_BSS} ${KERNEL_MAX_BSS} $<TARGET_FILE:MushCore_kernel>)
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/check_kernel_sectors.py ".rodata" ${KERNEL_START_RODATA} ${KERNEL_MAX_RODATA} $<TARGET_FILE:MushCore_kernel>)
add_custom_command (TARGET MushCore_kernel POST_BUILD COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/check_kernel_sectors.py ".text" ${KERNEL_START_TEXT} ${KERNEL_MAX_TEXT} $<TARGET_FILE:MushCore_kernel>)

# Add bootloader target
add_library (MushCore_bootloader OBJECT boot/loader.asm)
set_target_properties (MushCore_bootloader PROPERTIES NASM_OBJ_FORMAT bin)

# Merge resulting files to MushOS image file, setting 'MUSHOS_IMG' variable
add_custom_command (OUTPUT MushCore_complete.bin COMMAND ${CONCATENATE} $<TARGET_OBJECTS:MushCore_bootloader> $<TARGET_FILE:MushCore_kernel>.bin > MushCore_complete.bin COMMAND ${TRUNCATE} -s 4194304 MushCore_complete.bin DEPENDS MushCore_kernel MushCore_bootloader)
set (MUSHOS_IMG ${CMAKE_CURRENT_BINARY_DIR}/MushCore_complete.bin)
add_custom_target (MushCore DEPENDS MushCore_complete.bin)

# Copy system image to artifacts directory
add_custom_command (TARGET MushCore POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${MUSHOS_IMG}" "${ARTIFACTS_DIR}/MushOS.img")
