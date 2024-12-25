%include "../include/sconst.inc"

INT_VECTOR_SYSCALL equ 0x30

_NR_printx      equ 0
_NR_sendrec     equ 1

; 导出函数
global printx
global sendrec

bits 32
[SECTION .text]
; PUBLIC int printx(char* str);
printx:
	mov eax, _NR_printx
	mov edx, [esp + 4]
	int INT_VECTOR_SYSCALL
    ret

; PUBLIC int sendrec(int function, int src_dst, struct message* p_msg);
sendrec:
	mov eax, _NR_sendrec
	mov ebx, [esp + 4]
	mov ecx, [esp + 8]
	mov edx, [esp + 12]
	int INT_VECTOR_SYSCALL
	ret