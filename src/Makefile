# Canalotus

# 别名
OUTPUT_FILE = ../Canalotus/Canalotus/
INTERMEDIATE_FILE = ../Canalotus/intermediate/
IMG = ../Canalotus/img/
BOOT = $(OUTPUT_FILE)boot/boot.bin $(OUTPUT_FILE)boot/loader.bin
KERNEL = $(OUTPUT_FILE)kernel/kernel.bin
FLOPPY = /mnt/floppy/

OBJ = $(INTERMEDIATE_FILE)kernel/kernel.o\
	  $(INTERMEDIATE_FILE)kernel/string.o\
	  $(INTERMEDIATE_FILE)kernel/start.o\
	  $(INTERMEDIATE_FILE)lib/kliba.o\

# 编译器&链接器
ASM = nasm
C = gcc
LD = ld

# 编译器&链接器参数
LD_FLAGS = -s -m elf_i386 -Ttext 0x30400

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
	mkdir -p -m 700 ../Canalotus/intermediate/boot
	mkdir -p -m 700 ../Canalotus/intermediate/kernel
	mkdir -p -m 700 ../Canalotus/intermediate/lib
	mkdir -p -m 700 ../Canalotus/img

build: $(BOOT) $(KERNEL)

make_image:
	dd if=/dev/zero of=$(IMG)a.img bs=512 count=2880
	dd if=$(OUTPUT_FILE)boot/boot.bin of=$(IMG)a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG)a.img $(FLOPPY)
	sudo cp -fv $(OUTPUT_FILE)boot/loader.bin $(FLOPPY)
	sudo cp -fv $(OUTPUT_FILE)kernel/kernel.bin $(FLOPPY)
	sudo umount $(FLOPPY)

# 编译
$(OUTPUT_FILE)boot/boot.bin: boot/boot.asm
	$(ASM) -I boot $< -o $@

$(OUTPUT_FILE)boot/loader.bin: boot/loader.asm
	$(ASM) -I boot $< -o $@

$(INTERMEDIATE_FILE)kernel/string.o: lib/string.asm
	$(ASM) -f elf $< -o $@

$(INTERMEDIATE_FILE)kernel/start.o: kernel/start.c
	$(C) -c -m32 -fno-builtin -o $@ $<

$(INTERMEDIATE_FILE)kernel/kernel.o: kernel/kernel.asm
	$(ASM) -f elf -o $@ $<

$(INTERMEDIATE_FILE)lib/kliba.o: lib/kliba.asm
	$(ASM) -f elf -o $@ $<

$(KERNEL): $(OBJ)
	$(LD) $(LD_FLAGS) -o $@ $(OBJ)