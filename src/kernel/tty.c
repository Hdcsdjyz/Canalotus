/***
 * @file tty.c
 * @author Lhxl
 * @data 2024-12-28
 * @version build30
 * @brief 终端
 ***/

#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/keyboard.h"
#include "../include/sys/tty.h"
#include "../include/sys/global.h"
#include "../include/sys/color.h"
#include "../include/assert.h"

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

PUBLIC int syscall_printx(int _unused1, int _unused2, char* s, struct process* p_proc)
{
	const char* p;
	char ch;
	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;
	if (k_reenter == 0)
	{
		p = va2la(proc2pid(p_proc), s);
	}
	else if (k_reenter > 0)
	{
		p = s;
	}
	else
	{
		p = reenter_err;
	}
	if ((*p == MAG_CH_PANIC) || (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_SYSU_PROCS]))
	{
		disable_int();
		char* v = (char*)VMEM_SIZE;
		const char* q = p + 1;
		while (v < (char*)(VMEM_BASE + VMEM_SIZE))
		{
			*v++ = *q++;
			*v++ = RED;
			if (!*q)
			{
				while (((int)v - VMEM_BASE) % (SCREEN_WIDTH * 16))
				{
					v++;
					*v++ = MAKE_COLOR(BRIGHT, BLACK);
				}
				q = p + 1;
			}
		}
		__asm__ __volatile__("hlt");
	}
	while ((ch = *p++) != 0)
	{
		if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
		{
			continue;
		}
		out_char(tty_table[p_proc->nr_tty].p_console, ch);
	}
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