cmake_minimum_required (VERSION 3.7.0)

find_package (LLVM REQUIRED CONFIG)
message (STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message (STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories (${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library (compiler_lib 
        AST.cpp
        AST.h
        parser.cpp
        parser.h
        scanner.cpp
        scanner.h
        token.cpp
        token.h
		visualizer.cpp
		visualizer.h
		codegen.cpp
		codegen.h)

target_include_directories (compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(compiler_lib LLVM)