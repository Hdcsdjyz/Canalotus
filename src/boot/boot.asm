org 0x7C00

BaseOfStack equ 0x7C00
BaseOfLoader equ 0x9000
OffsetOfLoader equ 0x100
RootDirSectors equ 14
SectorNoOfRootDir equ 19

jmp short LABEL_START
nop

; FAT12头
BS_OEMName db 'LinHengX'
BPB_BytePerSec dw 512           ; 每扇区字节数
BPB_SecPerClus db 1             ; 每簇扇区数
BPB_RsvdSecCnt dw 1             ; Boot占用扇区数
BPB_NumFATs    db 2             ; FAT表数量
BPB_RootEntCnt dw 224           ; 根目录文件数最大值
BPB_TotSec16   dw 2880          ; 逻辑扇区数
BPB_Media      db 0xF0          ; 媒体描述符
BPB_FATSz16    dw 9             ; 每FAT扇区数
BPB_SecPerTrk  dw 18            ; 每磁道扇区数
BPB_NumHeads   dw 2             ; 磁头数
BPB_HiddSec    dd 0             ; 影藏扇区数
BPB_TotSec32   dd 0             ; 扇区数
BS_DrvNum      db 0             ; 中断0x13的驱动器号
BS_Reservedl   db 0             ; 保留
BS_BootSig     db 0x29          ; 扩展引导标记
BS_VolID       dd 0             ; 卷序列号
BS_VolLab      db 'Canalotus01' ; 卷标
BS_FileSysType db 'CL_FAT12'    ; 文件系统类型

; Boot
LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BaseOfStack
	call DispStr
	xor ah, ah
	xor dl, dl
	int 0x13

; 加载Loader
	mov word [wSectorNo], SectorNoOfRootDir
LABEL_SEARCH_IN_ROOTDIR_BEGIN:
	cmp word [wRootDirSizeForLoop], 0
	jz LABEL_NO_LOADERBIN
	dec word [wRootDirSizeForLoop]
	mov ax, BaseOfLoader
	mov es, ax
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
	mov dh, 2
	call DispStr
	jmp $
LABEL_FILE_FOUND:
	jmp $

; 变量
wRootDirSizeForLoop dw RootDirSectors
wSectorNo           dw 0
bOdd                db 0
LoaderFileName      db "LOADER  BIN", 0
MsgLen equ 9
BootMsg db "Booting. "
Msg1    db "Ready.   "
Msg2    db "NO LOADER"

; DispStr
DispStr:
	mov ax, MsgLen
	mul dh
	add ax, BootMsg
	mov bp, ax
	mov ax, ds
	mov es, ax
	mov cx, MsgLen
	mov ax, 0x1301
	mov bx, 0x0007
	mov dl, 0
	int 0x10
	ret

; 读扇区
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

times 510 - ($ - $$) db 0
dw 0xAA55