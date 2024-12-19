#include "include/proto.h"
#include "include/proc.h"
#include "include/global.h"
#include "include/string.h"

void TestA();

PUBLIC int kernel_main()
{
	disp_str("-----kernel_main() starts-----");

	struct task* p_task = task_table;
	struct process* p_proc = proc_table;
	u8* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++)
	{
		strcpy(p_proc->p_name, p_task->name);
		p_proc->pid = i;
		p_proc->ldt_sel = selector_ldt;
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_SYSU << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_SYSU << 5;

		p_proc->regs.cs = 0 & SA_RPL_MASK & SA_TI_MASK | SA_TIL | RPL_SYSU;
		p_proc->regs.ds = 8 & SA_RPL_MASK & SA_TI_MASK | SA_TIL | RPL_SYSU;
		p_proc->regs.es = 8 & SA_RPL_MASK & SA_TI_MASK | SA_TIL | RPL_SYSU;
		p_proc->regs.fs = 8 & SA_RPL_MASK & SA_TI_MASK | SA_TIL | RPL_SYSU;
		p_proc->regs.ss = 8 & SA_RPL_MASK & SA_TI_MASK | SA_TIL | RPL_SYSU;
		p_proc->regs.gs = SELECTOR_KERNEL_GS & SA_RPL_MASK | RPL_SYSU;
		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}


	k_reenter = -1;
	p_proc_ready = proc_table;
	restart();
	while (1)
	{

	}
}

void TestA()
{
	int i = 0;
	while (1)
	{
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}

void TestB()
{
	int i = 0x1000;
	while (1)
	{
		disp_str("B");
		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}