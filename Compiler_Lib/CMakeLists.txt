cmake_minimum_required (VERSION 3.7.0)

add_library (compiler_lib 
        AST.cpp
        AST.h
        interpreter.cpp
        interpreter.h
        parser.cpp
        parser.h
        scanner.cpp
        scanner.h
        token.cpp
        token.h
)
target_include_directories (compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
