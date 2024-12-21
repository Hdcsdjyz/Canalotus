#include "include/const.h"
#include "include/proto.h"
#include "include/global.h"

PUBLIC void clock_handler(int irq)
{
	ticks++;
	p_proc_ready->ticks--;
	if (k_reenter != 0)
	{
		return;
	}
	if (p_proc_ready->ticks > 0)
	{
		return;
	}
	schedule();
}

PUBLIC void milli_delay(int ms)
{
	int t = get_ticks();
	while (((get_ticks() - t) * 1000 / HZ) < ms)
	{

	}
}