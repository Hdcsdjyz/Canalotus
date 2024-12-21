/***
 * @file clock.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 时钟处理程序
 ***/

#include "include/const.h"
#include "include/proto.h"
#include "include/global.h"

PUBLIC void init_clock()
{
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8)(TIMER_FREQUENCY / HZ));
	out_byte(TIMER0, (u8)(TIMER_FREQUENCY / HZ >> 8));
	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

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