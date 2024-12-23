/***
 * @file proc.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 进程调度
 ***/

#include "include/const.h"
#include "include/proto.h"
#include "include/global.h"

PUBLIC int syscall_get_ticks()
{
	return ticks;
}

PUBLIC void schedule()
{
	struct process* p;
	int greatest_ticks = 0;
	while (!greatest_ticks)
	{
		for (p = proc_table; p < proc_table + NR_SYSU_PROCS + NR_USER_PROCS; p++)
		{
			if (p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}
		if (!greatest_ticks)
		{
			for (p = proc_table; p < proc_table + NR_SYSU_PROCS + NR_USER_PROCS; p++)
			{
				p->ticks = p->priority;
			}
		}
	}
}