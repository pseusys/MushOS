include (../compile_functions.cmake)

# Function for adding 'IN_FILE' contents to 'OUT_FILE', while dropping 'pattern'-matching lines
function (pipelines IN_FILE OUT_FILE pattern)
    file (STRINGS ${IN_FILE} LINES)
    foreach (LINE IN LISTS LINES)
        # Sets BLANK if source file line was blank
        if ("${LINE}" STREQUAL "")
            set (BLANK true)
        else ()
            set (BLANK false)
        endif ()
        # Replaces 'pattern'-matching lines
        string (REGEX REPLACE ${pattern} "" STRIPPED "${LINE}")
        # Adds new line only if line is not replaced with blank
        if (BLANK OR (NOT "${STRIPPED}" STREQUAL ""))
            string (APPEND STRIPPED "\n")
        endif ()
        # Writes result to file
        file (APPEND ${OUT_FILE} "${STRIPPED}")
    endforeach ()
endfunction ()


# Enable C and ASM compilers, C compiler should support cross-compiling, ASM compiler should support NASM
enable_language(C ASM_NASM)

# Source sets (C and ASM)
file (GLOB_RECURSE ASM_SOURCES code/*.asm)
file (GLOB_RECURSE C_SOURCES code/*.c)

# Headers, 'generic.h' header should always come first to be first in library header
file (GLOB GENERIC_HEADER code/generic.h)
file (GLOB_RECURSE HEADERS code/*.h)
list (FILTER HEADERS EXCLUDE REGEX "code/generic\\.h$")
list (PREPEND HEADERS ${GENERIC_HEADER})

# Folder where library generated files will be placed, situated in project
set (CMAKE_CURRENT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/binxero)

set (LIBRARY_HEADER ${CMAKE_CURRENT_BINARY_DIR}/mushlib.h)
file (WRITE ${LIBRARY_HEADER} "#ifndef MUSHLIB\n#define MUSHLIB\n")
foreach (SOURCE ${HEADERS})
    pipelines (${SOURCE} ${LIBRARY_HEADER} "^(#include.*|#ifndef.*|#define\ MUSHLIB.*|#endif.*)")
endforeach (SOURCE)
file (APPEND ${LIBRARY_HEADER} "\n#endif // MUSHLIB")

# TODO: somehow add HEADER_FILE_ONLY
add_library(mushlib STATIC ${C_SOURCES} ${ASM_SOURCES} ${LIBRARY_HEADER})
set_target_properties(mushlib PROPERTIES LINKER_LANGUAGE C)
