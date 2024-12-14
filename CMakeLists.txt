cmake_minimum_required(VERSION 3.28)
project(Canalotus ASM C)

enable_language(ASM C)

set(CMAKE_ASM_NASM_COMPILER "usr/bin/nasm")
set(CMAKE_ASM_NASM_FLAGS "-o boot/$(SOURCE_NAME)")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-c -nostdlib")

include_directories(src/boot/include)
add_executable(Canalotus src/boot/boot.asm src/boot/loader.asm)
add_library(kernel.o OBJECT src/boot/kernel.asm)

