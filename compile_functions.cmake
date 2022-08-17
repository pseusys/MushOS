# Function for adding 'IN_FILE' contents to 'OUT_FILE', while dropping 'pattern'-matching lines
function (crosscompile_c SOURCES HEADERS OBJECTS)
    list (JOIN HEADERS " " HEADERS_LINE)
    foreach (SOURCE ${SOURCES})
        GET_FILENAME_COMPONENT (FILE_BASE ${SOURCE} NAME_WE)
        SET (OBJECT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${FILE_BASE}.o)
        ADD_CUSTOM_COMMAND (OUTPUT ${OBJECT_FILE} COMMAND "gcc -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -ffreestanding -o ${OBJECT_FILE} -c ${SOURCE}")
        list (APPEND COMPILED_OBJECTS "${OBJECT_FILE}")
    endforeach ()
    SET (${OBJECTS} "${COMPILED_OBJECTS}" PARENT_SCOPE)
endfunction ()


# Function for adding 'IN_FILE' contents to 'OUT_FILE', while dropping 'pattern'-matching lines
function (crosscompile_asm SOURCES OBJECTS)
    foreach (SOURCE ${SOURCES})
        GET_FILENAME_COMPONENT (FILE_BASE ${SOURCE} NAME_WE)
        SET (OBJECT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${FILE_BASE}.o)
        ADD_CUSTOM_COMMAND (OUTPUT ${OBJECT_FILE} COMMAND "gcc -nostartfiles -nodefaultlibs -nolibc -nostdlib -DMUSHENV -m32 -ffreestanding -o ${OBJECT_FILE} -c ${SOURCE}")
        list (APPEND COMPILED_OBJECTS "${OBJECT_FILE}")
    endforeach ()
    SET (${OBJECTS} "${COMPILED_OBJECTS}" PARENT_SCOPE)
endfunction ()
