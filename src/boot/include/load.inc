; boot阶段用到的一些常量

BaseOfLoader        equ 0x9000  ; loader.bin被加载的段地址
OffsetOfLoader      equ 0x100   ; loader.bin被加载的偏移量
BaseOfLoaderPhyAddr equ BaseOfLoader * 0x10 ; loader.bin被加载的物理地址

BaseOfKernel    equ 0x8000 ; kernel.bin被加载的段地址
OffsetOfKernel  equ 0x0  ; kernel.bin被加载的偏移量

PageDirBase equ 0x200000 ; 页目录地址
PageTblBase equ 0x201000 ; 页表地址

BaseOfKernelPhyAddr equ BaseOfKernel * 0x10
KernelEntryPhyAddr  equ 0x30400

