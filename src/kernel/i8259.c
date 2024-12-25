/***
 * @file i8259.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 8259A中断芯片设置
 ***/

#include "../include/type.h"
#include "../include/const.h"
#include "../include/protect.h"
#include "../include/proto.h"
#include "../include/global.h"

/* 初始化8259A */
PUBLIC void init_8259A()
{
	out_byte(INT_M_CTL, (u8)0x11);
	out_byte(INT_S_CTL, (u8)0x11);
	out_byte(INT_M_CTLMASK, (u8)INT_VECTOR_IRQ0);
	out_byte(INT_S_CTLMASK, (u8)INT_VECTOR_IRQ8);
	out_byte(INT_M_CTLMASK, (u8)0x04);
	out_byte(INT_S_CTLMASK, (u8)0x02);
	out_byte(INT_M_CTLMASK, (u8)0x01);
	out_byte(INT_S_CTLMASK, (u8)0x01);
	out_byte(INT_M_CTLMASK, (u8)0xFF);
	out_byte(INT_S_CTLMASK, (u8)0xFF);

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

/***
 * @param[in] irq int 中断号
 * @param[in] handler (void*) (int) 中断程序
 ***/
PUBLIC void put_irq_handler(int irq, irq_handler handler)
{
	disable_irq(irq);
	irq_table[irq] = handler;
}