cmake_minimum_required (VERSION 3.7.0)
project (compiler)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory (Compiler_Lib)
add_subdirectory (Compiler_exe)