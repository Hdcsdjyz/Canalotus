/***
 * @file keyboard.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 键盘输入处理程序
 ***/

#include "include/const.h"
#include "include/proto.h"
#include "include/keyboard.h"
#include "include/string.h"
#include "include/keymap.h"

PRIVATE struct keyboard kbd;

/* 启用键盘中断 */
PUBLIC void init_keyboard()
{
	kbd.count = 0;
	kbd.p_head = kbd.p_tail = kbd.buf;
	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}

PUBLIC void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(KBD_DATA);
	if (kbd.count < KBD_INBYTES)
	{
		*kbd.p_head = scan_code;
		kbd.p_head++;
		if (kbd.p_head == kbd.buf + KBD_INBYTES)
		{
			kbd.p_head = kbd.buf;
		}
		kbd.count++;
	}
}

PUBLIC void keyboard_read()
{
	u8 scan_code;
	char output[2];
	u8 make;
	memset(output, 0, 2);
	if (kbd.count > 0)
	{
		disable_int();
		scan_code = *(kbd.p_tail);
		kbd.p_tail++;
		if (kbd.p_tail == kbd.buf + KBD_INBYTES)
		{
			kbd.p_tail = kbd.buf;
		}
		kbd.count--;
		enable_int();

		if (scan_code == 0xE1)
		{

		}
		else if (scan_code == 0xE0)
		{

		}
		else // 可打印字符
		{
			make = scan_code & FLAG_BREAK ? FALSE : TRUE;
			if (make)
			{
				output[0] = keymap[(scan_code & 0x7F) * MAP_COLS];
				disp_str(output);
			}
		}
	}
}