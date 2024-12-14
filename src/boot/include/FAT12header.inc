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

; 此FAT12格式软盘的一些常量
FATSz equ 9                     ; FAT占用扇区数
RootDirSectors equ 14           ; 根目录占用空间
SectorNoOfRootDir equ 19        ; 根目录的第一个扇区号
SectorNoOfFAT1 equ 1            ; FAT1的第一个扇区号
DeltaSectorNo equ 17            ; 文件开始的扇区号
