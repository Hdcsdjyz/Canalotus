%include "../include/sconst.inc"

; 导入变量
extern disp_pos

[SECTION .text]

; 导出函数
global disp_str
global out_byte
global in_byte
global disp_color_str
global disable_irq
global enable_irq
global disable_int
global enable_int

; void disp_str(char* str);
; 在屏幕上打印str
disp_str:
	push ebp
	mov ebp, esp
	mov esi, [ebp + 8]
	mov edi, [disp_pos]
	mov ah, 0x0F
.1:
	lodsb
	test al, al
	jz .2
	cmp al, 0x0A
	jnz .3
	push eax
	mov eax, edi
	mov bl, 160
	div bl
	and eax, 0xFF
	inc eax
	mov bl, 160
	mul bl
	mov edi, eax
	pop eax
	jmp .1
.3:
	mov [gs:edi], ax
	add edi, 2
	jmp .1
.2:
	mov [disp_pos], edi
	pop ebp
	ret

; void out_byte(u16 port, u8 value);
; 向端口输出一个字节value
out_byte:
	mov edx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	nop
	nop
	ret

; u8 in_byte(u16 port);
; 从端口输入一个字节
in_byte:
	mov edx, [esp + 4]
	xor eax, eax
	in al, dx
	nop
	nop
	ret

; PUBLIC void disp_color_str(char* str, u8 color);
disp_color_str:
	push ebp
	mov ebp, esp
	mov esi, [ebp + 8]
	mov edi, [disp_pos]
	mov ah, [ebp + 12]
.1:
	lodsb
	test al, al
	jz .2
	cmp al, 0x0A
	jnz .3
	push eax
	mov eax, edi
	mov bl, 160
	div bl
	and eax, 0xFF
	inc eax
	mov bl, 160
	mul bl
	mov edi, eax
	pop eax
	jmp .1
.3:
	mov [gs:edi], ax
	add edi, 2
	jmp .1
.2:
	mov [disp_pos], edi
	pop ebp
	ret

; void disable_irq(int irq);
; 禁用一个中断
disable_irq:
	mov ecx, [esp + 4]
	pushf
	cli
	mov ah, 1
	rol ah, cl
	cmp cl, 8
	jae disable_8
disable_0:
	in al, INT_M_CTLMASK
	test al, ah
	jnz dis_already
	or al, ah
	out INT_M_CTLMASK, al
	popf
	mov eax, 1
	ret
disable_8:
	in al, INT_S_CTLMASK
	test al, ah
	jnz dis_already
	or al, ah
	out INT_S_CTLMASK, al
	popf
	mov eax, 1
	ret
dis_already:
	popf
	xor eax, eax
	ret

; void enable_irq(int irq);
; 启用一个中断
enable_irq:
	mov ecx, [esp + 4]
	pushf
	cli
	mov ah, ~1
	rol ah, cl
	cmp cl, 8
	jae enable_8
enable_0:
	in al, INT_M_CTLMASK
	and al, ah
	out INT_M_CTLMASK, al
	popf
	ret
enable_8:
	in al, INT_S_CTLMASK
	and al, ah
	out INT_S_CTLMASK, al
	popf
	ret

disable_int:
	cli
	ret

enable_int:
	sti
	ret