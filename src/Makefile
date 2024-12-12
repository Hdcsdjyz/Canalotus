# alias
OUTPUT_FILE = ../Canalotus/Canalotus/
INTERMEDIATE_FILe = ../Canalotus/intermediate/
IMG = ../Canalotus/img/
BOOT = $(OUTPUT_FILE)boot/boot.bin $(OUTPUT_FILE)boot/loader.bin

# compiler
ASM = nasm

# .PHONY
.PHONY: default delete_file make_file build make_image

default: delete make_file build make_image

delete:
	rm -rf ../Canalotus

make_file:
	mkdir -p -m 700 ../Canalotus
	mkdir -p -m 700 ../Canalotus/Canalotus
	mkdir -p -m 700 ../Canalotus/Canalotus/boot
	mkdir -p -m 700 ../Canalotus/intermediate
	mkdir -p -m 700 ../Canalotus/img

build: $(BOOT)

make_image:
	dd if=/dev/zero of=$(IMG)a.img bs=512 count=2880
	dd if=$(OUTPUT_FILE)boot/boot.bin of=$(IMG)a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG)a.img /mnt/floppy
	sudo cp -v $(OUTPUT_FILE)boot/loader.bin /mnt/floppy
	sudo umount /mnt/floppy/

# compile
	@# asm
$(OUTPUT_FILE)boot/boot.bin: boot/boot.asm
	$(ASM) $< -o $@

$(OUTPUT_FILE)boot/loader.bin: boot/loader.asm
	$(ASM) $< -o $@
