cmake_minimum_required(VERSION 3.28)
project(Canalotus C)

enable_language(ASM C)

set(CMAKE_ASM_NASM_COMPILER "usr/bin/nasm")
set(CMAKE_ASM_NASM_FLAGS "-o boot/$(SOURCE_NAME)")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-c -nostdlib")

add_executable(Canalotus boot/boot.asm boot/loader.asm)
