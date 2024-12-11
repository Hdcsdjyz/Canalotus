# alias
OUTPUT_FILE = ../Canalotus/Canalotus/
INTERMEDIATE_FILe = ../Canalotus/intermediate/
IMG = ../Canalotus/img/
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

build: $(OUTPUT_FILE)boot/boot.bin

make_image:
	dd if=$(OUTPUT_FILE)boot/boot.bin of=$(IMG)a.img bs=512 count=1

$(OUTPUT_FILE)boot/boot.bin: boot/boot.asm
	$(ASM) $< -o $@
