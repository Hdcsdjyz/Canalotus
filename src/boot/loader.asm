org 0x100

BaseOfStack equ 0x100
BaseOfKernel equ 0x8000 ; kernel.bin被加载的段地址
OffsetOfKernel equ 0x0  ; kernel.bin被加载的偏移量

jmp LABEL_START

%include "FAT12head.inc"

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BaseOfStack

	mov dh, 0
	call DispStr

; 索引kernel.bin
; 与boot.asm中几乎一致
	mov word [wSectorNo], SectorNoOfRootDir
	xor ah, ah
	xor dl, dl
	int 0x13
LABEL_SEARCH_IN_ROOTDIR_BEGIN:
	cmp word [wRootDirSizeForLoop], 0
	jz LABEL_NO_KERNELBIN
	dec word [wRootDirSizeForLoop]
	mov ax, BaseOfKernel
	mov es, ax
	mov bx, OffsetOfKernel
	mov ax, [wSectorNo]
	mov cl, 1
	call ReadSector

	mov si, KernelFileName
	mov di, OffsetOfKernel
	cld
	mov dx, 0x10
LABEL_SEARCH_FOR_KERNELBIN:
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
	mov si, KernelFileName
	jmp LABEL_SEARCH_FOR_KERNELBIN
LABEL_GOTO_NEXT_SECTOR_IN_ROOTDIR:
	add word [wSectorNo], 1
	jmp LABEL_SEARCH_IN_ROOTDIR_BEGIN
LABEL_NO_KERNELBIN:
	mov dh, 2
	call DispStr
	jmp $
LABEL_FILE_FOUND:
	mov ax, RootDirSectors
	and di, 0xFFE0
	push eax
	mov eax, [es:di + 0x1C]
	mov dword [dwKernelSize], eax
	pop eax
	add di, 0x1A
	mov cx, word [es:di]
	push cx
	add cx, ax
	add cx, DeltaSectorNo
	mov ax, BaseOfKernel
	mov es, ax
	mov bx, OffsetOfKernel
	mov ax, cx
LABEL_LOAD_FILE:
	push ax
	push bx
	mov ah, 0x0E
	mov al, '.'
	mov bl, 0x0F
	int 0x10
	pop bx
	pop ax

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
	call KillMotor
	mov dh, 1
	call DispStr
	jmp $

; 函数：DispStr
DispStr:
	mov ax, MsgLen
	mul dh
	add ax, Msg0
	mov bp, ax
	mov ax, ds
	mov es, ax
	mov cx, MsgLen
	mov ax, 0x1301
	mov bx, 0x0007
	mov dl, 0
	add dh, 3
	int 0x10
	ret

; 变量
wRootDirSizeForLoop dw RootDirSectors
wSectorNo           dw 0
bOdd                db 0
dwKernelSize        dd 0
KernelFileName      db "KERNEL  BIN", 0
MsgLen equ 9
Msg0    db "Loading  "
Msg1    db "Ready.   "
Msg2    db "NO Kernel"

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
	mov ax, BaseOfKernel
	sub ax, 0x100
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
	and ax, 0xFFF
LABEL_GET_FAT_ENTRY_OK:
	pop bx
	pop es
	ret

; 关闭软驱马达
KillMotor:
	push dx
	mov dx, 0x3F2
	mov al, 0
	out dx, al
	pop dx
	ret