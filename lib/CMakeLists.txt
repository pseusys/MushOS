# Macro for replacing a source from `base` MushLib with some external source
macro (replace_source SOURCE source_list file_extension)
    get_filename_component (source_we ${SOURCE} NAME_WE)
    get_filename_component (source_abs ${SOURCE} ABSOLUTE)
    get_filename_component (replacing "base/${source_we}\.${file_extension}" ABSOLUTE)
    list (TRANSFORM ${source_list} REPLACE "${replacing}$" "${source_abs}")
endmacro ()


# Setup MushLib header file name
set (MUSHLIB_HEADER_FILE_NAME "mushlib.h")

# Find C and ASM source sets
file (GLOB_RECURSE SOURCES base/*.c base/*.asm)

# Create headers list, 'generic.h' header should always come first to be first in library header
file (GLOB GENERIC_HEADER base/generic.h)
file (GLOB_RECURSE HEADERS base/*.h)
list (FILTER HEADERS EXCLUDE REGEX "base/generic\\.h$")
list (PREPEND HEADERS ${GENERIC_HEADER})

# Replace ASM sources defined by user
if (DEFINED ASM_SOURCES)
    foreach (SOURCE ${ASM_SOURCES})
        replace_source (${SOURCE} SOURCES "asm")
    endforeach (SOURCE)
endif ()

# Replace C sources defined by user
if (DEFINED C_SOURCES)
    foreach (SOURCE ${C_SOURCES})
        replace_source (${SOURCE} SOURCES "c")
    endforeach (SOURCE)
endif ()

# Replace H sources defined by user
if (DEFINED H_SOURCES)
    foreach (SOURCE ${H_SOURCES})
        replace_source (${SOURCE} HEADERS "h")
    endforeach (SOURCE)
endif ()


# Generate common library header, that includes all library headers, smartly merged
add_custom_command (OUTPUT ${MUSHLIB_HEADER_FILE_NAME} COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/scripts/build_mushlib_header.py -l ${HEADERS} > ${MUSHLIB_HEADER_FILE_NAME} DEPENDS ${HEADERS})
add_custom_target (MushLibHeader DEPENDS ${MUSHLIB_HEADER_FILE_NAME})


# Add 'MushLib' library, it should be linked as C, it is also marked header only
add_library (MushLib STATIC ${SOURCES} ${LIBRARY_HEADER})
add_dependencies (MushLib MushLibHeader)
set_target_properties (MushLib PROPERTIES LINKER_LANGUAGE C HEADER_FILE_ONLY ON)

# Set 'MUSHLIB_HEAD' and 'MUSHLIB_BIN' variables
set (MUSHLIB_HEAD "${CMAKE_CURRENT_BINARY_DIR}/${MUSHLIB_HEADER_FILE_NAME}")
set (MUSHLIB_BIN $<TARGET_FILE:MushLib>)

# Copy mushlib binary and header to artifacts directory
add_custom_command (TARGET MushLib POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${MUSHLIB_HEAD}" "${ARTIFACTS_DIR}/mushlib.h")
add_custom_command (TARGET MushLib POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${MUSHLIB_BIN}" "${ARTIFACTS_DIR}/mushlib.a")
