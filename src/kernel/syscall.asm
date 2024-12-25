%include "../include/sconst.inc"

INT_VECTOR_SYSCALL equ 0x30

_NR_get_ticks equ 0
_NR_write equ 1

; 导出函数
global get_ticks
global write

bits 32
[SECTION .text]
; PUBLIC int get_ticks();
get_ticks:
	mov eax, _NR_get_ticks
	int INT_VECTOR_SYSCALL
	ret

; PUBLIC void write(char* buf, int len);
write:
	mov eax, _NR_write
	mov ebx, [esp + 4]
	mov ecx, [esp + 8]
	int INT_VECTOR_SYSCALL
	ret

sendrec:
	mov eax, _NR_sendrec
	mov ebp, [esp + 4]
	mov ecx, [esp + 8]
	mov edx, [esp + 12]
	int INT_VECTOR_SYSCALL
	ret