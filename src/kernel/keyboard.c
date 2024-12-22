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

PRIVATE u8 code_E0 = FALSE;
PRIVATE u8 lshift = FALSE;
PRIVATE u8 rshift = FALSE;
PRIVATE u8 lalt = FALSE;
PRIVATE u8 ralt = FALSE;
PRIVATE u8 lctrl = FALSE;
PRIVATE u8 rctrl = FALSE;
PRIVATE u8 caps_lock = FALSE;
PRIVATE u8 num_lock = FALSE;
PRIVATE u8 scroll_lock = FALSE;
PRIVATE u8 column = 0;

/* 本地函数 */
PRIVATE u8 get_byte_from_kbdbuf();

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
	u16 key = 0;
	u16* keyrow;

	if (kbd.count > 0)
	{
		disable_int();
		scan_code = get_byte_from_kbdbuf();

		if (scan_code == 0xE1)
		{
			u8 pausebreak_scode[] = {
				0xE1, 0x1D, 0x45,
				0xE1, 0x9D, 0xC5
			};
			u8 is_pausebreak = TRUE;
			for (int i = 1; i < 6; i++)
			{
				if (get_byte_from_kbdbuf() != pausebreak_scode[i])
				{
					is_pausebreak = FALSE;
					break;
				}
			}
			if (is_pausebreak)
			{
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0)
		{
			scan_code = get_byte_from_kbdbuf();
			if (scan_code == 0x2A)
			{
				if (get_byte_from_kbdbuf() == 0xE0)
				{
					if (get_byte_from_kbdbuf() == 0x37)
					{
						key = PRINTSCREEN;
						make = TRUE;
					}
				}
			}
			else if (scan_code == 0xB7)
			{
				if (get_byte_from_kbdbuf() == 0xE0)
				{
					if (get_byte_from_kbdbuf() == 0xAA)
					{
						key = PRINTSCREEN;
						make = FALSE;
					}
				}
			}
			if (key == 0)
			{
				code_E0 = TRUE;
			}
		}// 可打印字符
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN))
		{
			make = scan_code & FLAG_BREAK ? FALSE : TRUE;
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];
			column = 0;
			if (lshift || rshift)
			{
				column = 1;
			}
			if (code_E0)
			{
				column = 2;
				code_E0 = FALSE;
			}
			key = keyrow[column];
			switch (key)
			{
			case LSHIFT:
				{
					lshift = make;
					key = 0;
					break;
				}
			case RSHIFT:
				{
					rshift = make;
					key = 0;
					break;
				}
			case LCTRL:
				{
					lctrl = make;
					key = 0;
					break;
				}
			case RCTRL:
				{
					rctrl = make;
					key = 0;
					break;
				}
			case LALT:
				{
					lalt = make;
					key = 0;
					break;
				}
			case RALT:
				{
					ralt = make;
					key = 0;
					break;
				}
			default:
				{
					break;
				}
			}
			if (make)
			{
				key |= lshift ? FLAG_LSHIFT : 0;
				key |= rshift ? FLAG_RSHIFT : 0;
				key |= lctrl ? FLAG_LCTRL : 0;
				key |= rctrl ? FLAG_RCTRL : 0;
				key |= lalt ? FLAG_LALT : 0;
				key |= ralt ? FLAG_RALT : 0;

				in_process(key);
			}
		}
	}
}

/***
 * @return ·unsigned char· 键盘扫描码
 ***/
PRIVATE u8 get_byte_from_kbdbuf()
{
	u8 scan_code;
	while (kbd.count <= 0)
	{

	}

	disable_int();
	scan_code = *kbd.p_tail;
	kbd.p_tail++;
	if (kbd.p_tail == kbd.buf + KBD_INBYTES)
	{
		kbd.p_tail = kbd.buf;
	}
	kbd.count--;
	enable_int();
	return scan_code;
}