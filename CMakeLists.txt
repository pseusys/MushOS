cmake_minimum_required(VERSION 3.16)
project(MushOS C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE mush_core core/boot/* core/drivers/* core/kernel/*)
file(GLOB_RECURSE mush_lib lib/*)
add_executable(MushOS ${mush_core} ${mush_lib})
