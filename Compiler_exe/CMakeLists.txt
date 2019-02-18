cmake_minimum_required (VERSION 3.7.0)
project (compiler_exe)

# Debugging on windows
if( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.15" )
    add_compile_options( /JMC )
endif()

#llvm_map_components_to_libnames(llvm_libs all)

add_executable (compiler_exe Compiler_exe.cpp getopt.h getopt.cpp)
target_link_libraries (compiler_exe LINK_PUBLIC LLVM compiler_lib)
