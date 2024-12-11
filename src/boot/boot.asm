org 0x7C00
mov ax, cs
mov ds, ax
mov es, ax
call DispStr
jmp $

DispStr:
	mov ax, BOOT_MSG
	mov bp, ax
	mov cx, 16
	mov ax, 0x1301
	mov bx, 0x000C
	mov dl, 0
	int 0x10
	ret

BOOT_MSG: db "HelloOSWorld"

times 510 - ($ - $$) db 0
dw 0xAA55