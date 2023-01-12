include (CTest)

set (CMAKE_C_LINK_EXECUTABLE ${DEFAULT_C_LINK_EXECUTABLE})
set (CMAKE_C_FLAGS "-m32")

# Find integrator sources - MushLib-with-STDLIB compatibility provider
file (GLOB_RECURSE integrator integrator.*)

# Test "MushLib Base: String"
add_executable (MushLibBaseString lib/string.c ${integrator})
target_link_libraries(MushLibBaseString MushLib)
add_test (NAME "MushLib Base: String" COMMAND MushLibBaseString)

# Test "MushLib Base: Format"
add_executable (MushLibBaseFormat lib/format.c ${integrator})
target_link_libraries(MushLibBaseFormat MushLib)
add_test (NAME "MushLib Base: Format" COMMAND MushLibBaseFormat)



# Target for building all tests
add_custom_target (MushTest DEPENDS MushLib MushLibBaseString MushLibBaseFormat)
