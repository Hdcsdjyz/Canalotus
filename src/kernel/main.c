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
	/* 初始化进程 */
	struct process* p_proc = proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;

	struct task* p_task;
	u8 privilege;
	u8 rpl;
	u32 eflags;
	for (int i = 0; i < NR_SYSU_PROCS + NR_USER_PROCS; i++)
	{
		if (i < NR_SYSU_PROCS)
		{
			p_task = sysu_proc_table + i;
			privilege = PRIVILEGE_SYSU;
			rpl = RPL_SYSU;
			eflags = 0x1202;
		}
		else
		{
			p_task = user_proc_table + i - NR_SYSU_PROCS;
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202;
		}
		strcpy(p_proc->p_name, p_task->name);
		p_proc->pid = i;
		p_proc->ldt_sel = selector_ldt;
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;

		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	proc_table[0].ticks = proc_table[0].priority = 5;
	proc_table[1].ticks = proc_table[1].priority = 20;
	proc_table[2].ticks = proc_table[2].priority = 20;
	proc_table[3].ticks = proc_table[3].priority = 20;

	proc_table[1].nr_tty = 0;
	proc_table[2].nr_tty = 1;
	proc_table[3].nr_tty = 2;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;
	/* 设置中断程序 */
	init_clock();
	init_keyboard();
	restart();
	while (1)
	{

	}
}

void TestA()
{
	while (1)
	{
		printf("<A:%x>", get_ticks());
		milli_delay(100);
	}
}

void TestB()
{
	while (1)
	{
		printf("<B:%x>", get_ticks());
		milli_delay(100);
	}
}

void TestC()
{
	while (1)
	{
		printf("<C:%x>", get_ticks());
		milli_delay(100);
	}
}