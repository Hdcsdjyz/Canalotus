org 0x7C00
jmp short LABEL_START
nop

%include "include/FAT12header.inc"
%include "include/load.inc"

BaseOfStack equ 0x7C00

; Boot
LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BaseOfStack

	; 清屏
	mov ax, 0x0600
	mov bx, 0x0700
	mov cx, 0
	mov dx, 0x184F
	int 0x10

	; 索引loader.bin
	xor ah, ah
	xor dl, dl
	int 0x13
	mov word [wSectorNo], SectorNoOfRootDir
LABEL_SEARCH_IN_ROOTDIR_BEGIN:
	cmp word [wRootDirSizeForLoop], 0
	jz LABEL_NO_LOADERBIN
	dec word [wRootDirSizeForLoop]
	mov ax, BaseOfLoader
	mov bx, OffsetOfLoader
	mov ax, [wSectorNo]
	mov cl, 1
	call ReadSector

	mov si, LoaderFileName
	mov di, OffsetOfLoader
	cld
	mov dx, 0x10
LABEL_SEARCH_FOR_LOADERBIN:
	cmp dx, 0
	jz LABEL_GOTO_NEXT_SECTOR_IN_ROOTDIR
	dec dx
	mov cx, 11
LABEL_CMP_FILENAME:
	cmp cx, 0
	jz LABEL_FILE_FOUND
	dec cx
	lodsb
	cmp al, byte [es:di]
	jz LABEL_GO_ON
	jmp LABEL_DIFFERENT
LABEL_GO_ON:
	inc di
	jmp LABEL_CMP_FILENAME
LABEL_DIFFERENT:
	and di, 0xFFE0
	add di, 0x20
	mov si, LoaderFileName
	jmp LABEL_SEARCH_FOR_LOADERBIN
LABEL_GOTO_NEXT_SECTOR_IN_ROOTDIR:
	add word [wSectorNo], 1
	jmp LABEL_SEARCH_IN_ROOTDIR_BEGIN
LABEL_NO_LOADERBIN:
	jmp $
LABEL_FILE_FOUND:
	mov ax, RootDirSectors
	and di, 0xFFE0
	add di, 0x1A
	mov cx, word [es:di]
	push cx
	add cx, ax
	add cx, DeltaSectorNo
	mov ax, BaseOfLoader
	mov es, ax
	mov bx, OffsetOfLoader
	mov ax, cx
LABEL_LOAD_FILE:
	mov cl, 1
	call ReadSector
	pop ax
	call GetFATEntry
	cmp ax, 0x0FFF
	jz LABEL_FILE_LOADED
	push ax
	mov dx, RootDirSectors
	add ax, dx
	add ax, DeltaSectorNo
	add bx, [BPB_BytePerSec]
	jmp LABEL_LOAD_FILE

LABEL_FILE_LOADED:
	jmp BaseOfLoader:OffsetOfLoader

; 变量
wRootDirSizeForLoop dw RootDirSectors
wSectorNo           dw 0
bOdd                db 0
LoaderFileName      db "LOADER  BIN", 0

; 函数：ReadSector
; 读取扇区
ReadSector:
	push bp
	mov bp, sp
	sub esp, 2
	mov byte [bp - 2], cl
	push bx
	mov bl, [BPB_SecPerTrk]
	div bl
	inc ah
	mov cl, ah
	mov dh, al
	shr al, 1
	mov ch, al
	and dh, 1
	pop bx
	mov dl, [BS_DrvNum]
.GoOnReading:
	mov ah, 2
	mov al, byte [bp - 2]
	int 0x13
	jc .GoOnReading
	add esp, 2
	pop bp
	ret

; 函数：GetFATEntry
GetFATEntry:
	push es
	push bx
	push ax
	mov ax, BaseOfLoader
	sub ax, 0x0100
	mov es, ax
	pop ax
	mov byte [bOdd], 0
	mov bx, 3
	mul bx
	mov bx, 2
	div bx
	cmp dx, 0
	jz LABEL_EVEN
	mov byte [bOdd], 1
LABEL_EVEN:
	xor dx, dx
	mov bx, [BPB_BytePerSec]
	div bx
	push dx
	mov bx, 0
	add ax, SectorNoOfFAT1
	mov cl, 2
	call ReadSector
	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [bOdd], 1
	jnz LABEL_EVEN_2
	shr ax, 4
LABEL_EVEN_2:
	and ax, 0x0FFF
LABEL_GET_FAT_ENTRY_OK:
	pop bx
	pop es
	ret

times 510 - ($ - $$) db 0
dw 0xAA55