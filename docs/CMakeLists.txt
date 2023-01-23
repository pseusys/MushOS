# DRAWIO - should be path to `drawio` utility
if (NOT DEFINED DRAWIO)
    set (DRAWIO drawio)
endif ()
# BASH - should be path to `bash` utility
if (NOT DEFINED BASH)
    set (BASH bash)
endif ()
# CURL - should be path to `curl` utility
if (NOT DEFINED CURL)
    set (CURL curl)
endif ()
# GREP - should be path to `grep` utility
if (NOT DEFINED GREP)
    set (GREP grep)
endif ()
# CUT - should be path to `cut` utility
if (NOT DEFINED CUT)
    set (CUT cut)
endif ()
# WGET - should be path to `wget` utility
if (NOT DEFINED WGET)
    set (WGET wget)
endif ()
# UNZIP - should be path to `unzip` utility
if (NOT DEFINED UNZIP)
    set (UNZIP unzip)
endif ()
# DOXYGEN - should be path to `doxygen` utility
if (NOT DEFINED DOXYGEN)
    set (DOXYGEN doxygen)
endif ()


# Find .drawio image sources
file (GLOB_RECURSE IMAGE_SOURCES assets/*.drawio)

# Generate list of .png image output paths and setup generation rules
set (IMAGE_OUTPUTS "")
foreach (SOURCE ${IMAGE_SOURCES})
    get_filename_component (source_we ${SOURCE} NAME_WE)
    get_filename_component (OUTPUT "assets/${source_we}\.png" ABSOLUTE)
    if (NOT EXISTS ${OUTPUT})
        add_custom_command (OUTPUT ${OUTPUT} COMMAND ${DRAWIO} -x -f png -o ${OUTPUT} ${SOURCE})
    endif ()
    list (APPEND IMAGE_OUTPUTS ${OUTPUT})
endforeach (SOURCE)


#
execute_process (COMMAND ${BASH} "-c" "${CURL} -s 'https://api.github.com/repos/jothepro/doxygen-awesome-css/releases/latest' | ${GREP} zipball_url | ${CUT} -d '\"' -f 4 -z" OUTPUT_VARIABLE THEME_URL)
add_custom_command (OUTPUT "theme.zip" COMMAND ${BASH} "-c" "${WGET} -O theme.zip '${THEME_URL}'")
add_custom_command (OUTPUT "theme" COMMAND ${BASH} "-c" "${UNZIP} -j -d theme theme.zip '\*.js' '\*.css'" DEPENDS "theme.zip")

#
set (DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/html)
add_custom_command (OUTPUT ${DOXYGEN_OUTPUT_DIR} COMMAND ${DOXYGEN} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile DEPENDS "theme" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#
add_custom_target (MushDoc DEPENDS ${IMAGE_OUTPUTS} ${DOXYGEN_OUTPUT_DIR})
