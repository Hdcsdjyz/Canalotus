/***
 * @file keyboard.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 键盘输入处理程序
 ***/

#include "../include/const.h"
#include "../include/proto.h"
#include "../include/keyboard.h"
#include "../include/keymap.h"

PRIVATE struct keyboard kbd;

PRIVATE u8 code_E0;
PRIVATE u8 lshift;
PRIVATE u8 rshift;
PRIVATE u8 lalt;
PRIVATE u8 ralt;
PRIVATE u8 lctrl;
PRIVATE u8 rctrl;
PRIVATE u8 caps_lock;
PRIVATE u8 num_lock;
PRIVATE u8 scroll_lock;
PRIVATE u8 column;

/* 本地函数 */
PRIVATE u8 get_byte_from_kbdbuf();
PRIVATE void kbd_wait();
PRIVATE void kbd_ack();
PRIVATE void set_leds();

/* 启用键盘中断 */
PUBLIC void init_keyboard()
{
	kbd.count = 0;
	kbd.p_head = kbd.p_tail = kbd.buf;

	lshift = rshift = FALSE;
	lalt = ralt = FALSE;
	lctrl = rctrl = FALSE;
	caps_lock = FALSE;
	num_lock = FALSE;
	scroll_lock = FALSE;
	set_leds();

	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}

PUBLIC void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(KBD_DATA);
	if (kbd.count < KBD_IN_BYTES)
	{
		*kbd.p_head = scan_code;
		kbd.p_head++;
		if (kbd.p_head == kbd.buf + KBD_IN_BYTES)
		{
			kbd.p_head = kbd.buf;
		}
		kbd.count++;
	}
}

PUBLIC void keyboard_read(struct tty* p_tty)
{
	u8 scan_code;
	u8 make;
	u16 key = 0;
	u16* keyrow;

	if (kbd.count > 0)
	{
		code_E0 = FALSE;
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
			u8 caps = lshift || rshift;
			if (caps_lock)
			{
				if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z'))
				{
					caps = !caps;
				}
			}
			if (caps)
			{
				column = 1;
			}
			if (code_E0)
			{
				column = 2;
			}
			key = keyrow[column];
			switch (key)
			{
			case LSHIFT:
				lshift = make;
				break;
			case RSHIFT:
				rshift = make;
				break;
			case LCTRL:
				lctrl = make;
				break;
			case RCTRL:
				rctrl = make;
				break;
			case LALT:
				lalt = make;
				break;
			case RALT:
				ralt = make;
				break;
			case CAPS_LOCK:
				if (make)
				{
					caps_lock = !caps_lock;
					set_leds();
				}
				break;
			case NUM_LOCK:
				if (make)
				{
					num_lock = !num_lock;
					set_leds();
				}
				break;
			case SCROLL_LOCK:
				if (make)
				{
					scroll_lock = !scroll_lock;
					set_leds();
				}
				break;
			default:
				break;
			}
			if (make)
			{
				u8 pad = FALSE;
				if ((key >= PAD_SLASH) && (key <= PAD_9))
				{
					pad = TRUE;
				}
				switch (key)
				{
				case PAD_SLASH:
					key = '/';
					break;
				case PAD_STAR:
					key = '*';
					break;
				case PAD_MINUS:
					key = '-';
					break;
				case PAD_PLUS:
					key = '+';
					break;
				case PAD_ENTER:
					key = ENTER;
					break;
				default:
					if (num_lock && (key >= PAD_0) && (key <= PAD_9))
					{
						key = key - PAD_0 + '0';
					}
					else if (num_lock && (key == PAD_DOT))
					{
						key = '.';
					}
					else
					{
						switch (key)
						{
						case PAD_HOME:
							key = HOME;
							break;
						case PAD_UP:
							key = UP;
							break;
						case PAD_PAGEUP:
							key = PAGEUP;
							break;
						case PAD_LEFT:
							key = LEFT;
							break;
						case PAD_RIGHT:
							key = RIGHT;
							break;
						case PAD_END:
							key = END;
							break;
						case PAD_DOWN:
							key = DOWN;
							break;
						case PAD_PAGEDOWN:
							key = PAGEDOWN;
							break;
						case PAD_INS:
							key = INSERT;
							break;
						case PAD_DEL:
							key = DELETE;
							break;
						default:
							break;
						}
					break;
					}
				}
				key |= lshift ? FLAG_LSHIFT : 0;
				key |= rshift ? FLAG_RSHIFT : 0;
				key |= lctrl ? FLAG_LCTRL : 0;
				key |= rctrl ? FLAG_RCTRL : 0;
				key |= lalt ? FLAG_LALT : 0;
				key |= ralt ? FLAG_RALT : 0;
				key |= pad ? FLAG_PAD : 0;
				in_process(p_tty, key);
			}
		}
	}
}

/***
 * @return unsigned char ~ 键盘扫描码
 ***/
PRIVATE u8 get_byte_from_kbdbuf()
{
	while (kbd.count <= 0)
	{

	}
	disable_int();
	u8 scan_code = *kbd.p_tail;
	kbd.p_tail++;
	if (kbd.p_tail == kbd.buf + KBD_IN_BYTES)
	{
		kbd.p_tail = kbd.buf;
	}
	kbd.count--;
	enable_int();
	return scan_code;
}

PRIVATE void kbd_wait()
{
	u8 kbd_stat;
	do
	{
		kbd_stat = in_byte(KBD_CMD);
	} while (kbd_stat & 0x02);
}

PRIVATE void kbd_ack()
{
	u8 kbd_read;
	do
	{
		kbd_read = in_byte(KBD_DATA);
	} while (kbd_read != KBD_ACK);
}

PRIVATE void set_leds()
{
	u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;
	kbd_wait();
	out_byte(KBD_DATA, LED_CODE);
	kbd_ack();
	kbd_wait();
	out_byte(KBD_DATA, leds);
	kbd_ack();
}