# Compiler
Hopefully this will be a small compiler for a turing complete language

### To-do list
 - [ ] Fix chained if/else statements
 - [ ] Add boolean AND/OR/NOT operators
 - [ ] Add option for debugging information in builds
 - [ ] code generation for ternary operator
 - [ ] Add strings

## Docs
See the wiki for implementation notes and bits about the language, which I've called SIMPLE.  Example programs used for testing can be found [here](test/Test%20programs)

## Installing LLVM Dependencies
This is a pain
### Linux
Installing the LLVM libraries depends on the distro/package manager.
#### Arch
Install LLVM 7 (with libraries) with `sudo pacman -S llvm` or just libraries with `sudo pacman -S llvm-libs`.
#### Debian/Ubuntu
Follow the instructions at http://apt.llvm.org/.
Use `sudo add-apt-repository "deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-7 main"` then `sudo apt install llvm-7` for LLVM 7 on Strtch for example.

## Building
This project uses cmake, so it should be straightforward
Make sure you are in the build directory, then `cmake .. && make`

### Windows
There is no support for linking LLVM on Windows because I have no idea how to make it work.
~~Just open the folder in visual studio if you are using it and it has support for cmake projects, or use cmake CLI/GUI to generate the solution files in the build directory and then open~~
