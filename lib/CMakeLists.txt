# Function for adding 'IN_FILE' contents to 'result', while dropping 'pattern'-matching lines
function (pipelines IN_FILE pattern result)
    set (header_lines "")
    file (STRINGS ${IN_FILE} LINES)
    foreach (LINE IN LISTS LINES)
        # Set BLANK if source file line was blank
        if ("${LINE}" STREQUAL "")
            set (BLANK true)
        else ()
            set (BLANK false)
        endif ()
        # Replace 'pattern'-matching lines
        string (REGEX REPLACE ${pattern} "" STRIPPED "${LINE}")
        # Add new line only if line is not replaced with blank
        if (BLANK OR (NOT "${STRIPPED}" STREQUAL ""))
            string (APPEND STRIPPED "\n")
        endif ()
        # Write result to 'header_lines'
        set (header_lines "${header_lines}${STRIPPED}")
    endforeach ()
    # Write result to 'result'
    set (${result} "${header_lines}" PARENT_SCOPE)
endfunction ()


# Enable C and ASM compilers, C compiler should support cross-compiling, ASM compiler should support NASM
enable_language (C ASM_NASM)
set (MUSHLIB_HEADER_FILE_NAME "mushlib.h")

# Define integer and floating point types size for MushLib as it requires constant length types.
add_compile_options ("$<$<COMPILE_LANG_AND_ID:C,GNU>:-DCHAR_TYPE_SIZE=8;-DSHORT_TYPE_SIZE=16;-DINT_TYPE_SIZE=32;-DLONG_TYPE_SIZE=64;-DLONG_LONG_TYPE_SIZE=128>")
add_compile_options ("$<$<COMPILE_LANG_AND_ID:C,GNU>:-DFLOAT_TYPE_SIZE=64;-DDOUBLE_TYPE_SIZE=128;-DLONG_DOUBLE_TYPE_SIZE=256>")

# Find source sets (C and ASM)
file (GLOB_RECURSE ASM_SOURCES code/*.asm)
file (GLOB_RECURSE C_SOURCES code/*.c)

# Create headers list, 'generic.h' header should always come first to be first in library header
file (GLOB GENERIC_HEADER code/generic.h)
file (GLOB_RECURSE HEADERS code/*.h)
list (FILTER HEADERS EXCLUDE REGEX "code/generic\\.h$")
list (PREPEND HEADERS ${GENERIC_HEADER})

# Generate common library header, that includes all library headers, smartly merged
set (header_result "#ifndef MUSHLIB\n#define MUSHLIB\n")
foreach (SOURCE ${HEADERS})
    set (header_merge "")
    pipelines (${SOURCE} "^(#include.*|#ifndef.*|#define\ MUSHLIB.*|#endif.*)" header_merge)
    set (header_result "${header_result}${header_merge}")
endforeach (SOURCE)
set (header_result "${header_result}\n#endif // MUSHLIB\n")
file (GENERATE OUTPUT ${MUSHLIB_HEADER_FILE_NAME} CONTENT "${header_result}")

# Add 'MushLib' library, it should be linked as C (C and ASM linkers are the same), it is also marked header only
add_library (MushLib STATIC ${C_SOURCES} ${ASM_SOURCES} ${LIBRARY_HEADER})
set_target_properties (MushLib PROPERTIES LINKER_LANGUAGE C HEADER_FILE_ONLY ON)

# Set 'MUSHLIB_HEAD' and 'MUSHLIB_BIN' variables
set (MUSHLIB_HEAD "${CMAKE_CURRENT_BINARY_DIR}/${MUSHLIB_HEADER_FILE_NAME}" PARENT_SCOPE)
set (MUSHLIB_BIN $<TARGET_FILE:MushLib> PARENT_SCOPE)
