org 0x100
jmp LABEL_START
nop

%include "include/FAT12header.inc"
%include "include/macro.inc"
%include "include/load.inc"

; GDT
LABEL_GDT:          Descriptor 0,           0,          0
LABEL_DESC_FLAT_C:  Descriptor 0,           0xFFFFF,    DA_CR | DA_32 | DA_LIMIT_4K
LABEL_DESC_FLAT_RW: Descriptor 0,           0xFFFFF,    DA_DRW | DA_32 | DA_LIMIT_4K
LABEL_DESC_VIDEO:   Descriptor 0xB8000,     0xFFFF,     DA_DRW | DA_DPL3

GdtLen equ $ - LABEL_GDT
GdtPtr dw GdtLen - 1
	   dd BaseOfLoaderPhyAddr + LABEL_GDT

; GDT选择子
Selector_FlatC      equ LABEL_DESC_FLAT_C - LABEL_GDT
Selector_FlatRW     equ LABEL_DESC_FLAT_RW - LABEL_GDT
Selector_Video      equ LABEL_DESC_VIDEO - LABEL_GDT + SA_RPL3

BaseOfStack equ 0x100   ; 栈底地址

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BaseOfStack

; 获取内存大小
	mov ebx, 0
	mov di, _MemChkBuf
.MemChkLoop:
	mov eax, 0xE820
	mov ecx, 20
	mov edx, 0x534D4150
	int 0x15
	jc .MemChkFail
	add di, 20
	inc dword [_dwMCRNumber]
	cmp ebx, 0
	jne .MemChkLoop
	jmp .MemChkOK
.MemChkFail:
	mov dword [_dwMCRNumber], 0

; 索引kernel.bin
; 与boot.asm中几乎一致
.MemChkOK:
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
	jmp $
LABEL_FILE_FOUND:
	mov ax, RootDirSectors
	and di, 0xFFF0
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

; 进入保护模式
	lgdt [GdtPtr]
	cli
	in al, 0x92
	or al, 0b00000010
	out 0x92, al
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword Selector_FlatC:(BaseOfLoaderPhyAddr + LABEL_PM_START)
	jmp $

; 变量
wRootDirSizeForLoop dw RootDirSectors
wSectorNo           dw 0
bOdd                db 0
dwKernelSize        dd 0
KernelFileName      db "KERNEL  BIN", 0

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
	and ax, 0x0FFF
LABEL_GET_FAT_ENTRY_OK:
	pop bx
	pop es
	ret

; 关闭软驱马达
KillMotor:
	push dx
	mov dx, 0x03F2
	mov al, 0
	out dx, al
	pop dx
	ret

[SECTION .s32]
ALIGN 32
[BITS 32]

LABEL_PM_START:
	mov ax, Selector_Video
	mov gs, ax
	mov ax, Selector_FlatRW
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov esp, TopOfStack

; 设置页
	add esp, 4
	call DispMemInfo
	call SetupPaging
	call InitKernel
	jmp Selector_FlatC:KernelEntryPhyAddr

MemCpy:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push ecx
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ecx, [ebp + 16]
.1:
	cmp ecx, 0
	jz .2
	mov al, [ds:esi]
	inc esi
	mov byte [es:edi], al
	inc edi
	dec ecx
	jmp .1
.2:
	mov eax, [ebp + 8]
	pop ecx
	pop edi
	pop esi
	mov esp, ebp
	pop ebp
	ret

; 填充ARDStruct
DispMemInfo:
	push esi
	push edi
	push ecx
	mov esi, MemChkBuf
	mov ecx, [dwMCRNumber]
.loop:
	mov edx, 5
	mov edi, ARDStruct
.1:
	mov eax, dword [esi]
	stosd
	add esi, 4
	dec edx
	cmp edx, 0
	jnz .1
	cmp dword [dwType], 1
	jne .2
	mov eax, [dwBaseAddrLow]
	add eax, [dwLengthLow]
	cmp eax, [dwMemSize]
	jb .2
	mov [dwMemSize], eax
.2:
	loop .loop
	pop ecx
	pop edi
	pop esi
	ret

; 启动并设置分页
SetupPaging:
	xor edx, edx
	mov eax, [dwMemSize]
	mov ebx, 0x400000
	div ebx
	mov ecx, eax
	test edx, edx
	jz .no_remainder
	inc ecx
.no_remainder:
	push ecx
	mov ax, Selector_FlatRW
	mov es, ax
	mov edi, PageDirBase
	xor eax, eax
	mov eax, PageTblBase | PG_P | PG_USU | PG_RWW
.1:
	stosd
	add eax, 4096
	loop .1
	pop eax
	mov ebx, 1024
	mul ebx
	mov ecx, eax
	mov edi, PageTblBase
	xor eax, eax
	mov eax, PG_P | PG_USU | PG_RWW
.2:
	stosd
	add eax, 4096
	loop .2
	mov eax, PageDirBase
	mov cr3, eax
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	jmp short .3
.3:
	nop
	ret

InitKernel:
	xor esi, esi
	mov cx, word [BaseOfKernelPhyAddr + 0x2C]
	movzx ecx, cx
	mov esi, [BaseOfKernelPhyAddr + 0x1C]
	add esi, BaseOfKernelPhyAddr
.Begin:
	mov eax, [esi + 0]
	cmp eax, 0
	jz .NoAction
	push dword [esi + 0x10]
	mov eax, [esi + 0x04]
	add eax, BaseOfKernelPhyAddr
	push eax
	push dword [esi + 0x08]
	call MemCpy
	add esp, 12
.NoAction:
	add esi, 0x20
	dec ecx
	jnz .Begin
	ret

[SECTION .data1]
ALIGN 32

LABEL_DATA:
; 变量
_dwMCRNumber: dd 0
_dwMemSize: dd 0
_ARDStruct:
	_dwBaseAddrLow:     dd 0
	_dwBaseAddrHigh:    dd 0
	_dwLengthLow:       dd 0
	_dwLengthHigh:      dd 0
	_dwType:            dd 0
_MemChkBuf: times 256 db 0

; 保护模式下用
dwMCRNumber:    equ BaseOfLoaderPhyAddr + _dwMCRNumber
dwMemSize:      equ BaseOfLoaderPhyAddr + _dwMemSize
ARDStruct:      equ BaseOfLoaderPhyAddr + _ARDStruct
	dwBaseAddrLow:      equ BaseOfLoaderPhyAddr + _dwBaseAddrLow
	dwBaseAddrHigh:     equ BaseOfLoaderPhyAddr + _dwBaseAddrHigh
	dwLengthLow:        equ BaseOfLoaderPhyAddr + _dwLengthLow
	dwLengthHigh:       equ BaseOfLoaderPhyAddr + _dwLengthHigh
	dwType:             equ BaseOfLoaderPhyAddr + _dwType
MemChkBuf:      equ BaseOfLoaderPhyAddr + _MemChkBuf

; 堆栈区
StackSpace: times 1024 db 0
TopOfStack equ BaseOfLoaderPhyAddr + $ ; 栈顶地址