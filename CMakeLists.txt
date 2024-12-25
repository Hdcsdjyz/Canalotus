cmake_minimum_required(VERSION 3.28)
project(Canalotus ASM C)

enable_language(ASM C)

set(CMAKE_ASM_NASM_COMPILER "usr/bin/nasm")
set(CMAKE_ASM_NASM_FLAGS "-o boot/$(SOURCE_NAME)")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "-c -nostdlib")

include_directories(src/boot/include)
add_executable(boot src/boot/boot.asm
        src/boot/include/FAT12header.inc
        src/boot/include/load.inc
        src/boot/include/macro.inc
)
add_executable(loader src/boot/loader.asm
        src/boot/include/FAT12header.inc
        src/boot/include/load.inc
        src/boot/include/macro.inc
)

add_executable(kernel src/kernel/kernel.asm
        src/include/color.h
        src/include/console.h
        src/include/const.h
        src/include/global.h
        src/include/keyboard.h
        src/include/keymap.h
        src/include/proc.h
        src/include/protect.h
        src/include/proto.h
        src/include/sconst.inc
        src/include/string.h
        src/include/tty.h
        src/include/type.h
)

add_library(start.o OBJECT src/kernel/start.c)
add_library(main.o OBJECT src/kernel/main.c)
add_library(i8259.o OBJECT src/kernel/i8259.c)
add_library(protect.o OBJECT src/kernel/protect.c)
add_library(global.o OBJECT src/kernel/global.c)
add_library(clock.o OBJECT src/kernel/clock.c)
add_library(syscall.o OBJECT src/kernel/syscall.asm)
add_library(proc.o OBJECT src/kernel/proc.c)
add_library(keyboard.o OBJECT src/kernel/keyboard.c)
add_library(tty.o OBJECT src/kernel/tty.c)
add_library(console.o OBJECT src/kernel/console.c)
add_library(printf.o OBJECT src/kernel/printf.c)
add_library(vsprintf.o OBJECT src/kernel/vsprintf.c)
add_library(kliba.o OBJECT src/lib/kliba.asm)
add_library(klib.o OBJECT src/lib/klib.c)
add_library(misc.o OBJECT src/lib/misc.c)
add_library(string.o OBJECT src/lib/string.asm)

include_directories(src/include)


