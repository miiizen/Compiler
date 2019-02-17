cmake_minimum_required (VERSION 3.7.0)

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