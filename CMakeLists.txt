cmake_minimum_required(VERSION 3.28)
project(Canalotus ASM C)

enable_language(ASM C)

set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")
set(CMAKE_ASM_NASM_FLAGS "-o boot/$(SOURCE_NAME)")
set(CMAKE_C_COMPILER "/usr/bin/gcc")
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
        src/include/sys/color.h
        src/include/sys/console.h
        src/include/sys/const.h
        src/include/sys/global.h
        src/include/sys/keyboard.h
        src/include/sys/keymap.h
        src/include/sys/proc.h
        src/include/sys/protect.h
        src/include/sys/proto.h
        src/include/sys/sconst.inc
        src/include/string.h
        src/include/sys/tty.h
        src/include/sys/type.h
        src/include/sys/hd.h
        src/include/sys/config.h
        src/include/sys/fs.h
        src/include/stdio.h
        src/include/assert.h
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
add_library(printf.o OBJECT src/lib/printf.c)
add_library(vsprintf.o OBJECT src/lib/vsprintf.c)
add_library(sys.o OBJECT src/kernel/sys.c)
add_library(hd.o OBJECT src/kernel/hd.c)
add_library(kliba.o OBJECT src/lib/kliba.asm)
add_library(klib.o OBJECT src/lib/klib.c)
add_library(misc.o OBJECT src/lib/misc.c)
add_library(string.o OBJECT src/lib/string.asm)
add_library(fs_main.o OBJECT src/fs/main.c)

include_directories(src/include)


