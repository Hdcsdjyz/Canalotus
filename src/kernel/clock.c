#include "include/const.h"
#include "include/proto.h"
#include "include/global.h"

PUBLIC void clock_handler(int irq)
{
	disp_str("#");
	p_proc_ready++;
	if (p_proc_ready >= proc_table + NR_TASKS)
	{
		p_proc_ready = proc_table;
	}
}