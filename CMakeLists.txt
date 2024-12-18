cmake_minimum_required(VERSION 3.28)
project(Canalotus ASM C)

enable_language(ASM C)

set(CMAKE_ASM_NASM_COMPILER "usr/bin/nasm")
set(CMAKE_ASM_NASM_FLAGS "-o boot/$(SOURCE_NAME)")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-c -nostdlib")

include_directories(src/boot/include)
add_executable(Canalotus src/boot/boot.asm src/boot/loader.asm)

add_library(kernel.o OBJECT src/kernel/kernel.asm)
add_library(start.o OBJECT src/kernel/start.c)
add_library(main.o OBJECT src/kernel/main.c)
add_library(i8259.o OBJECT src/kernel/i8259.c)
add_library(protect.o OBJECT src/kernel/protect.c)
add_library(global.o OBJECT src/kernel/global.c)
add_library(klib.o OBJECT src/lib/klib.c)
add_library(string.o OBJECT src/lib/string.asm)

include_directories(src/kernel/include)


