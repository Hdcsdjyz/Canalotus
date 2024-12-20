#include "include/global.h"
#include "include/proto.h"
#include "include/const.h"
#include "include/string.h"

/* 本地函数 */
PRIVATE void init_idt_desc(u8 vector, u8 desc_type, int_handler handler, u8 privilege);
PRIVATE void init_descriptor(struct descriptor* p_desc, u32 base, u32 limit, u16 attribute);

/* 异常处理程序 */
PUBLIC void exception_handler(int vec_no, int error_code, int eip, int cs, int eflags)
{
	int i;
	int text_color = 0x74;

	char* err_msg[] = {
		"#DE_divide_Error",
		"#DB_reserved",
		"#___nmi_interrupt",
		"#BP_breakpoint",
		"#OF_overflow",
		"#BR_bounds_range_exceeded",
		"#UD_undefined_opcode",
		"#NM_device_not_available",
		"#DF_double_fault",
		"#___coprocessor_segment_overrun",
		"#TS_invalid_TSS",
		"#NP_segment_not_present",
		"#SS_stack_segment_fault",
		"#GP_general_protection",
		"#PF_page_fault",
		"#DO NOT USE",
		"#MF_x87fpu_floatpoint_error",
		"#AC_alignment_check",
		"#MC_machine_check",
		"#XF_SIMD_floating_point_error",
	};

	disp_pos = 0;
	for (i = 0; i < 80 * 5; i++)
	{
		disp_str(" ");
	}
	disp_pos = 0;
	disp_color_str("Exception! --> ", text_color);
	disp_color_str(err_msg[vec_no], text_color);
	disp_color_str("\n\n", text_color);
	disp_color_str("EFLAGS:", text_color);
	disp_int(eflags);
	disp_color_str("CS:", text_color);
	disp_int(cs);
	disp_color_str("EIP:", text_color);
	disp_int(eip);
	if (error_code != 0xFFFFFFFF)
	{
		disp_color_str("Error code:", text_color);
		disp_int(error_code);
	}
}

void divide_error_handler();
void single_step_handler();
void nmi_handler();
void breakpoint_handler();
void overflow_handler();
void bounds_handler();
void invalid_opcode_handler();
void device_not_available_handler();
void double_fault_handler();
void coprocessor_segment_overrun_handler();
void invalid_tss_handler();
void segment_not_present_handler();
void stack_segment_fault_handler();
void general_protection_handler();
void page_fault_handler();
void coprocessor_error_handler();

void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

/* 设置中断处理程序、IDT */
PUBLIC void init_prot()
{
	init_8259A();

	/* 初始化异常处理程序 */
	init_idt_desc(INT_VECTOR_DIVIDE, DA_386IGate, divide_error_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DEBUG, DA_386IGate, single_step_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_NMI, DA_386IGate, nmi_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_OVERFLOW, DA_386IGate, overflow_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_BOUNDS, DA_386IGate, bounds_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_OP, DA_386IGate, invalid_opcode_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_NOT, DA_386IGate, device_not_available_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_SEG, DA_386IGate, coprocessor_segment_overrun_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_TSS, DA_386IGate, invalid_tss_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_segment_fault_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PROTECTION, DA_386IGate, general_protection_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault_handler, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_ERR, DA_386IGate, coprocessor_error_handler, PRIVILEGE_KRNL);

	/* 初始化硬件中断处理程序 */
	init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 8, DA_386IGate, hwint08, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 9, DA_386IGate, hwint09, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 10, DA_386IGate, hwint10, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 11, DA_386IGate, hwint11, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 12, DA_386IGate, hwint12, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 13, DA_386IGate, hwint13, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 14, DA_386IGate, hwint14, PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 15, DA_386IGate, hwint15, PRIVILEGE_KRNL);

	/* 初始化系统调用（软件中断处理程序） */
	init_idt_desc(INT_VECTOR_SYSCALL, DA_386IGate, _syscall, PRIVILEGE_USER);

	/* 初始化TSS */
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = SELECTOR_KERNEL_DS;
	init_descriptor(&gdt[INDEX_TSS], vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss), sizeof(tss) - 1, DA_386TSS);
	tss.iomap = sizeof(tss);

	/* 初始化进程的LDT */
	int i;
	struct process* p_proc = proc_table;
	u16 selector_ldt = INDEX_LDT_FIRST << 3;
	for (i = 0; i < NR_TASKS; i++)
	{
		init_descriptor(&gdt[selector_ldt >> 3], vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].ldts), LDT_SIZE * sizeof(struct descriptor) - 1, DA_LDT);
		p_proc++;
		selector_ldt += 1 << 3;
	}
}

/* 由段名求绝对地址 */
PUBLIC u32 seg2phys(u16 seg)
{
	struct descriptor* p_dst = &gdt[seg >> 3];
	return (p_dst->base_high << 24 | p_dst->base_mid << 16 | p_dst->base_low);
}

/* vvv 本地函数 vvv */

/*- 初始化中断门 */
PRIVATE void init_idt_desc(u8 vector, u8 desc_type, int_handler handler, u8 privilege)
{
	struct gate* p_gate = &idt[vector];
	u32 base = (u32)handler;
	p_gate->offset_low = base & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->dcount = 0;
	p_gate->attr = desc_type | privilege << 5;
	p_gate->offset_high = base >> 16 & 0xFFFF;
}

/*- 初始化段描述符 */
PRIVATE void init_descriptor(struct descriptor* p_desc, u32 base, u32 limit, u16 attribute)
{
	p_desc->limit_low = limit & 0xFFFF;
	p_desc->base_low = base & 0xFFFF;
	p_desc->base_mid = base >> 16 & 0xFF;
	p_desc->attr1 = attribute & 0xFF;
	p_desc->limit_high_attr2 = (attribute >> 16 & 0x0F) | (attribute >> 8 & 0xF0);
	p_desc->base_high = base >> 24 & 0xFF;
}
/* ^^^ 本地函数 ^^^ */