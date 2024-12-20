#include "include/type.h"
#include "include/const.h"
#include "include/protect.h"
#include "include/proto.h"
#include "include/global.h"

/* 初始化8259A */
PUBLIC void init_8259A()
{
	out_byte(INT_M_CTL, 0x11);
	out_byte(INT_S_CTL, 0x11);
	out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);
	out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);
	out_byte(INT_M_CTLMASK, 0x04);
	out_byte(INT_S_CTLMASK, 0x02);
	out_byte(INT_M_CTLMASK, 0x01);
	out_byte(INT_S_CTLMASK, 0x01);
	out_byte(INT_M_CTLMASK, 0xFF);
	out_byte(INT_S_CTLMASK, 0xFF);

	for (int i = 0; i < NR_IRQ; i++)
	{
		irq_table[i] = spurious_irq;
	}
}

PUBLIC void spurious_irq(int irq)
{
	disp_str("spurious_irq: ");
	disp_int(irq);
	disp_str("\n");
}

PUBLIC void put_irq_handler(int irq, irq_handler handler)
{
	disable_irq(irq);
	irq_table[irq] = handler;
}