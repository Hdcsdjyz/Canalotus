; 段属性
DA_32           equ 0x4000  ; 32位段
DA_LIMIT_4K     equ 0x8000  ; 段界限颗粒度为4KB

DA_DPL0         equ 0x00    ; DPL = 0
DA_DPL1         equ 0x20    ; DPL = 1
DA_DPL2         equ 0x40    ; DPL = 2
DA_DPL3         equ 0x60    ; DPL = 3

DA_DR           equ 0x90    ; 存在、只读、数据
DA_DRW          equ 0x92    ; 存在、可读写、数据
DA_DRWA         equ 0x93    ; 存在、已访问、可读写、数据
DA_C            equ 0x98    ; 存在、只执行、代码
DA_CR           equ 0x9A    ; 存在、可执行、可读、代码
DA_CCO          equ 0x9C    ; 存在、只执行、一致、代码
DA_CCOR         equ 0x9E    ; 存在、可执行、可读、一致、代码

DA_LDT          equ 0x82    ;
DA_TaskGate     equ 0x85    ; 任务门
DA_386TSS       equ 0x89    ; 状态段
DA_386CGate     equ 0x8C    ; 调用门
DA_386IGate     equ 0x8E    ; 中断门
DA_386TGate     equ 0x8F    ; 陷阱门

; 段选择子属性
SA_RPL0         equ 0x00    ; RPL = 0
SA_RPL1         equ 0x01    ; RPL = 1
SA_RPL2         equ 0x02    ; RPL = 2
SA_RPL3         equ 0x03    ; RPL = 3
SA_TIG          equ 0x00    ;
SA_TIL          equ 0x04    ;

; 页属性
PG_P            equ 0x01    ; 存在
PG_RWR          equ 0x00    ; 可读、可执行
PG_RWW          equ 0x02    ; 可读写、可执行
PG_USS          equ 0x00    ; 系统级
PG_USU          equ 0x04    ; 用户级

; 用法：Descriptor 段基址, 段界限, 段属性
%macro Descriptor 3
	dw %2 & 0xFFFF
	dw %1 & 0xFFFF
	db (%1 >> 16) & 0xFF
	dw ((%2 >> 8) & 0x0F00) | (%3 & 0xF0FF)
	db (%1 >> 24) & 0xFF
%endmacro

; 用法：Gate 段选择子, 偏移量, 参数数量, 属性
%macro Gate 4
	dw (%2 & 0xFFFF)
	dw %1
	dw (%3 & 0x1F) | ((%4 << 8) & 0xFF00)
	dw ((%2 >> 16) & 0xFFFF)
%endmacro