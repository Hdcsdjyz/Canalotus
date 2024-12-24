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
PRIVATE void put_key(struct tty* p_tty, u16 key);

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
	if (!(key & FLAG_EXT))
	{
		put_key(p_tty, key);
	}
	else
	{
		u16 raw_code = key & MASK_RAW;
		switch (raw_code)
		{
		case UP:
			if ((key & FLAG_LSHIFT) || (key & FLAG_RSHIFT))
			{
				scroll_screen(p_tty->p_console, SCREEN_UP);
			}
			break;
		case DOWN:
			if ((key & FLAG_LSHIFT) || (key & FLAG_RSHIFT))
			{
				scroll_screen(p_tty->p_console, SCREEN_DOWN);
			}
			break;
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
			if (key & FLAG_LALT || key & FLAG_RALT)
			{
				select_console(raw_code - F1);
			}
			break;
		case ENTER:
			put_key(p_tty, '\n');
			break;
		case BACKSPACE:
			put_key(p_tty, '\b');
			break;
		default:
			break;
		}
	}
}

PUBLIC void tty_write(struct tty* p_tty, char* buf, int len)
{
	char* p = buf;
	int i = len;
	while (i)
	{
		out_char(p_tty->p_console, *p++);
		i--;
	}
}

PUBLIC int syscall_write(char* buf, int len, struct process* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
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
		char ch = *p_tty->p_inbuf_tail;
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES)
		{
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;
		out_char(p_tty->p_console, ch);
	}
}

PRIVATE void put_key(struct tty* p_tty, u16 key)
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