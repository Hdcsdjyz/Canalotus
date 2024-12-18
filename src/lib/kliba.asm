extern disp_pos

[SECTION .text]

global disp_str
global out_byte
global in_byte
global disp_color_str

; void disp_str(char* str)
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

; void out_byte(u16 port, u8 value)
; 向端口输出一个字节value
out_byte:
	mov edx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	nop
	nop
	ret

; void in_byte(u16 port)
; 从端口输入一个字节
in_byte:
	mov edx, [esp + 4]
	xor eax, eax
	in al, dx
	nop
	nop
	ret

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