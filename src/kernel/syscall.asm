%include "include/sconst.inc"

_NR_get_ticks equ 0
INT_VECTOR_SYSCALL equ 0x30

; 导出函数
global get_ticks

bits 32
[SECTION .text]
get_ticks:
	mov eax, _NR_get_ticks
	int INT_VECTOR_SYSCALL
	ret