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
		for (p = proc_table; p < proc_table + NR_TASKS; p++)
		{
			if (p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}
		if (!greatest_ticks)
		{
			for (p = proc_table; p < proc_table + NR_TASKS; p++)
			{
				p->ticks = p->priority;
			}
		}
	}
}