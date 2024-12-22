/***
 * @file tty.c
 * @author Lhxl
 * @data 2024-12-22
 * @version 1.0
 * @brief 终端
 ***/

#include "include/const.h"
#include "include/proto.h"
#include "include/keyboard.h"
#include "include/tty.h"
#include "include/global.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(struct tty* p_tty);
PRIVATE void tty_do_read(struct tty* p_tty);
PRIVATE void tty_do_write(struct tty* p_tty);

PUBLIC void sys_tty()
{
	struct tty* p_tty;
	init_keyboard();
	for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
	{
		init_tty(p_tty);
	}
	select_console(0);
	while (1)
	{
		for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
		{
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}

PUBLIC void in_process(struct tty* p_tty, u16 key)
{
	u8 output[2] = {'\0', '\0'};
	if (!(key & FLAG_EXT))
	{
		if (p_tty->inbuf_count < TTY_IN_BYTES)
		{
			*p_tty->p_inbuf_head = key;
			p_tty->p_inbuf_head++;
			if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES)
			{
				p_tty->p_inbuf_head = p_tty->in_buf;
			}
			p_tty->inbuf_count++;
		}
	}
	else
	{
		u16 raw_code = key & MASK_RAW;
		switch (raw_code)
		{
		case UP:
			{
				if ((key & FLAG_LSHIFT) || (key & FLAG_RSHIFT))
				{
					disable_int();
					out_byte(CRTC_ADDR_REG, START_ADDR_H);
					out_byte(CRTC_DATA_REG, ((80 * 10) >> 8) & 0xFF);
					out_byte(CRTC_ADDR_REG, START_ADDR_L);
					out_byte(CRTC_DATA_REG, (80 * 10) & 0xFF);
					enable_int();
				}
				break;
			}
		case DOWN:
			{
				if ((key & FLAG_LSHIFT) || (key & FLAG_RSHIFT))
				{

				}
				break;
			}
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			{
				if (key & FLAG_LALT || key & FLAG_RALT)
				{
					select_console(raw_code - F1);
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
}

PUBLIC u8 is_current_console(struct console* p_console)
{
	return p_console == &console_table[nr_current_console];
}

/************/

PRIVATE void init_tty(struct tty* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;
	init_screen(p_tty);
}

PRIVATE void tty_do_read(struct tty* p_tty)
{
	if (is_current_console(p_tty->p_console))
	{
		keyboard_read(p_tty);
	}
}

PRIVATE void tty_do_write(struct tty* p_tty)
{
	if (p_tty->inbuf_count)
	{
		u8 ch = *p_tty->p_inbuf_tail;
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES)
		{
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;
		out_char(p_tty->p_console, ch);
	}
}