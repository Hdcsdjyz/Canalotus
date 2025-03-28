# Canalotus

# 别名
OUTPUT_FILE = ../Canalotus/Canalotus/
INTERMEDIATE_FILE = ../Canalotus/intermediate/
IMG = ../Canalotus/img/
BOOT = $(OUTPUT_FILE)boot/boot.bin $(OUTPUT_FILE)boot/loader.bin
KERNEL = $(OUTPUT_FILE)kernel/kernel.bin
FLOPPY = /mnt/floppy/

OBJ = $(INTERMEDIATE_FILE)kernel/kernel.o\
	  $(INTERMEDIATE_FILE)kernel/syscall.o\
	  $(INTERMEDIATE_FILE)kernel/start.o\
	  $(INTERMEDIATE_FILE)kernel/main.o\
	  $(INTERMEDIATE_FILE)kernel/clock.o\
	  $(INTERMEDIATE_FILE)kernel/keyboard.o\
	  $(INTERMEDIATE_FILE)kernel/tty.o\
	  $(INTERMEDIATE_FILE)kernel/console.o\
	  $(INTERMEDIATE_FILE)kernel/i8259.o\
	  $(INTERMEDIATE_FILE)kernel/global.o\
	  $(INTERMEDIATE_FILE)kernel/protect.o\
	  $(INTERMEDIATE_FILE)kernel/proc.o\
	  $(INTERMEDIATE_FILE)kernel/sys.o\
	  $(INTERMEDIATE_FILE)kernel/hd.o\
	  $(INTERMEDIATE_FILE)lib/kliba.o\
	  $(INTERMEDIATE_FILE)lib/klib.o\
	  $(INTERMEDIATE_FILE)lib/misc.o\
	  $(INTERMEDIATE_FILE)lib/string.o\
	  $(INTERMEDIATE_FILE)lib/vsprintf.o\
	  $(INTERMEDIATE_FILE)lib/printf.o\
	  $(INTERMEDIATE_FILE)fs/main.o\

# 编译器&链接器
ASM = nasm
C = gcc
LD = ld

# 编译器&链接器参数
C_FLAGS =  -c -m32 -fno-builtin -fno-pic -Wall -fno-stack-protector -D CL_DEBUG
LD_FLAGS = -s -m elf_i386 -Ttext 0x30400 -z noexecstack

# make选项
.PHONY: default delete_file make_file build make_image

default: delete make_file build make_image

delete:
	rm -rf ../Canalotus

make_file:
	mkdir -p -m 700 ../Canalotus
	mkdir -p -m 700 ../Canalotus/Canalotus
	mkdir -p -m 700 ../Canalotus/Canalotus/boot
	mkdir -p -m 700 ../Canalotus/Canalotus/kernel
	mkdir -p -m 700 ../Canalotus/intermediate
	mkdir -p -m 700 ../Canalotus/intermediate/kernel
	mkdir -p -m 700 ../Canalotus/intermediate/lib
	mkdir -p -m 700 ../Canalotus/intermediate/fs
	mkdir -p -m 700 ../Canalotus/img

build: $(BOOT) $(KERNEL)

make_image:
	dd if=/dev/zero of=$(IMG)a.img bs=512 count=2880
	dd if=$(OUTPUT_FILE)boot/boot.bin of=$(IMG)a.img bs=512 count=1 conv=notrunc
	mount -o loop $(IMG)a.img $(FLOPPY)
	cp -fv $(OUTPUT_FILE)boot/loader.bin $(FLOPPY)
	cp -fv $(OUTPUT_FILE)kernel/kernel.bin $(FLOPPY)
	umount $(FLOPPY)

# 编译
$(OUTPUT_FILE)boot/%.bin: boot/%.asm
	$(ASM) -I boot $< -o $@

$(INTERMEDIATE_FILE)kernel/%.o: kernel/%.asm
	$(ASM) -f elf -I kernel -o $@ $<

$(INTERMEDIATE_FILE)kernel/%.o: kernel/%.c
	$(C) $(C_FLAGS) -o $@ $<

$(INTERMEDIATE_FILE)lib/%.o: lib/%.asm
	$(ASM) -f elf -I lib -o $@ $<

$(INTERMEDIATE_FILE)lib/%.o: lib/%.c
	$(C) $(C_FLAGS) -o $@ $<

$(INTERMEDIATE_FILE)fs/%.o: fs/%.c
	$(C) $(C_FLAGS) -o $@ $<

$(KERNEL): $(OBJ)
	$(LD) $(LD_FLAGS) -o $@ $(OBJ)