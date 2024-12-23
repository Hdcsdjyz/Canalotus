/***
 * @file keyboard.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 内核主程序
 ***/

#include "include/proto.h"
#include "include/proc.h"
#include "include/global.h"
#include "include/string.h"
#include "include/color.h"

PUBLIC int kernel_main()
{
	/* 设置中断程序 */
	init_clock();
	init_keyboard();
	/* 初始化进程 */
	struct task* p_task = task_table;
	struct process* p_proc = proc_table;
	u8* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	for (int i = 0; i < NR_TASKS; i++)
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
	proc_table[0].ticks = proc_table[0].priority = 15;
	proc_table[1].ticks = proc_table[1].priority = 10;
	proc_table[2].ticks = proc_table[2].priority = 5;
	proc_table[3].ticks = proc_table[3].priority = 1;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;
	restart();
	while (1)
	{

	}
}

void TestA()
{
	while (1)
	{
		milli_delay(20);
	}
}

void TestB()
{
	while (1)
	{
		milli_delay(20);
	}
}

void TestC()
{
	while (1)
	{
		milli_delay(20);
	}
}