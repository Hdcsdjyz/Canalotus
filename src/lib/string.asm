[SECTION .text]

global memcpy
global memset
global strcpy

; void* memcpy(void* p_dst, void* p_src, int size);
memcpy:
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

; void memset(void* p_dst, char ch, int size);
memset:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push ecx
	mov edi, [ebp + 8]
	mov edx, [ebp + 12]
	mov ecx, [ebp + 16]
.1:
	cmp ecx, 0
	jz .2
	mov byte [edi], dl
	inc edi
	dec ecx
	jmp .1
.2:
	pop ecx
	pop edi
	pop esi
	mov esp ,ebp
	pop ebp
	ret

; char* strcpy(char* p_dst, char* p_src);
strcpy:
	push ebp
	mov ebp, esp
	mov esi, [ebp + 12]
	mov edi, [ebp + 8]
.1:
	mov al, [esi]
	inc esi
	mov byte [edi], al
	inc edi
	cmp al, 0
	jnz .1
	mov eax, [ebp + 8]
	pop ebp
	ret