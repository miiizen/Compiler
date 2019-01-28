cmake_minimum_required (VERSION 3.7.0)
project (compiler_exe)
add_executable (compiler_exe Compiler_exe.cpp)
target_link_libraries (compiler_exe LINK_PUBLIC compiler_lib)
