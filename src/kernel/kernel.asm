%include "include/sconst.inc"

; 导入函数
extern cstart
extern exception_handler
extern spurious_irq
extern kernel_main
extern disp_str
extern clock_handler
extern syscall_table

; 导入变量
extern gdt_ptr
extern idt_ptr
extern disp_pos
extern tss
extern p_proc_ready
extern k_reenter
extern irq_table

[SECTION .bss]
StackSpace resb 2 * 1024
StackTop:

[SECTION .text]
; 导出函数
global _start
global restart

; 导出变量
;; 异常
global divide_error_handler
global single_step_handler
global nmi_handler
global breakpoint_handler
global overflow_handler
global bounds_handler
global invalid_opcode_handler
global device_not_available_handler
global double_fault_handler
global coprocessor_segment_overrun_handler
global invalid_tss_handler
global segment_not_present_handler
global stack_segment_fault_handler
global general_protection_handler
global page_fault_handler
global coprocessor_error_handler

;;硬件中断
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

;; 系统调用（软件中断）
global _syscall

_start:
	mov esp, StackTop
	mov dword [disp_pos], 0
	sgdt [gdt_ptr]
	call cstart
	lgdt [gdt_ptr]
	lidt [idt_ptr]
	jmp SELECTOR_KERNEL_CS:csinit
csinit:
	xor eax, eax
	mov ax, SELECTOR_TSS
	ltr ax
	jmp kernel_main

; 异常
divide_error_handler:
	push 0xFFFFFFFF
	push 0
	jmp exception
single_step_handler:
	push 0xFFFFFFFF
	push 1
	jmp exception
nmi_handler:
	push 0xFFFFFFFF
    push 2
    jmp exception
breakpoint_handler:
	push 0xFFFFFFFF
    push 3
    jmp exception
overflow_handler:
	push 0xFFFFFFFF
    push 4
    jmp exception
bounds_handler:
	push 0xFFFFFFFF
    push 5
    jmp exception
invalid_opcode_handler:
	push 0xFFFFFFFF
    push 6
    jmp exception
device_not_available_handler:
	push 0xFFFFFFFF
    push 7
    jmp exception
double_fault_handler:
    push 8
    jmp exception
coprocessor_segment_overrun_handler:
	push 0xFFFFFFFF
    push 9
    jmp exception
invalid_tss_handler:
    push 10
    jmp exception
segment_not_present_handler:
    push 11
    jmp exception
stack_segment_fault_handler:
    push 12
    jmp exception
general_protection_handler:
    push 13
    jmp exception
page_fault_handler:
    push 14
    jmp exception
coprocessor_error_handler:
	push 0xFFFFFFFF
    push 16
    jmp exception
exception:
	call exception_handler
	add esp, 8
	hlt

; 默认中断（主）
%macro hwint_master 1
	call save
    in al, INT_M_CTLMASK
    or al, (1 << %1)
    out INT_M_CTLMASK, al
    mov al, EOI
    out INT_M_CTL, al
    sti
    push %1
    call [irq_table + 4 * %1]
    pop ecx
    cli
    in al, INT_M_CTLMASK
    and al, ~(1 << %1)
    out INT_M_CTLMASK, al
    ret
%endmacro

; irq0：时钟中断
ALIGN 16
hwint00:
	hwint_master 0
; irq2：键盘中断
ALIGN 16
hwint01:
	hwint_master 1
ALIGN 16
hwint02:
	hwint_master 2
ALIGN 16
hwint03:
	hwint_master 3
ALIGN 16
hwint04:
	hwint_master 4
ALIGN 16
hwint05:
	hwint_master 5
ALIGN 16
hwint06:
	hwint_master 6
ALIGN 16
hwint07:
	hwint_master 7

; 默认中断（从）
%macro hwint_slave 1
	push %1
	call spurious_irq
	add esp, 4
	hlt
%endmacro

ALIGN 16
hwint08:
	hwint_slave 8
ALIGN 16
hwint09:
	hwint_slave 9
ALIGN 16
hwint10:
	hwint_slave 10
ALIGN 16
hwint11:
	hwint_slave 11
ALIGN 16
hwint12:
	hwint_slave 12
ALIGN 16
hwint13:
	hwint_slave 13
ALIGN 16
hwint14:
	hwint_slave 14
ALIGN 16
hwint15:
	hwint_slave 15

_syscall:
	call save
	sti
	call [syscall_table + eax * 4]
	mov [esi + EAXREG - P_STACKBASE], eax
	cli
	ret

save:
	pushad
	push ds
	push es
	push fs
	push gs
	mov dx, ss
	mov ds, dx
	mov es, dx
	mov esi, esp
	inc dword [k_reenter]
	cmp dword [k_reenter], 0
	jne .1
	mov esp, StackTop
	push restart
	jmp [esi + RETADR - P_STACKBASE]
.1:
	push restart_reenter
	jmp [esi + RETADR - P_STACKBASE]

restart:
	mov esp, [p_proc_ready]
	lldt [esp + P_LDT_SEL]
	lea eax, [esp + P_STACKTOP]
	mov dword [tss + TSS3_S_SP0], eax

restart_reenter:
	dec dword [k_reenter]
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 4
	iretd