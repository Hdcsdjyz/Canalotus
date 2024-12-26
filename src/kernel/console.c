/***
 * @file console.c
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief 控制台
 ***/

#include "../include/proto.h"
#include "../include/global.h"
#include "../include/color.h"

PRIVATE void set_cursor(u32 pos);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(struct console* p_console);

PUBLIC void out_char(struct console* p_console, u8 ch)
{
	u8* p_vmem = (u8*)(VMEM_BASE + p_console->cursor * 2);
	switch (ch)
	{
	case '\n':
		{
			if (p_console->cursor < p_console->original_addr + p_console->vmem_limit - SCREEN_WIDTH)
			{
				p_console->cursor = p_console->original_addr + SCREEN_WIDTH * ((p_console->cursor - p_console->original_addr) / SCREEN_WIDTH + 1);
			}
			break;
		}
	case '\b':
		{
			if (p_console->cursor > p_console->original_addr)
			{
				p_console->cursor--;
				*(p_vmem - 2) = ' ';
				*(p_vmem - 1) = DEFAULT_CHAR_COLOR;
			}
			break;
		}
	default:
		{
			if (p_console->cursor < p_console->original_addr + p_console->vmem_limit - 1)
			{
				*p_vmem++ = ch;
				*p_vmem++ = DEFAULT_CHAR_COLOR;
				p_console->cursor++;
			}
			break;
		}
	}
	while (p_console->cursor >= p_console->current_start_addr + SCREEN_SIZE)
	{
		scroll_screen(p_console, SCREEN_DOWN);
	}
	flush(p_console);
}

PUBLIC void init_screen(struct tty* p_tty)
{
	u8 nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;
	u32 vmem_size = VMEM_SIZE >> 1;
	u32 console_vmem_size = vmem_size / NR_CONSOLES;
	p_tty->p_console->original_addr = nr_tty * console_vmem_size;
	p_tty->p_console->vmem_limit = console_vmem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;
	p_tty->p_console->cursor = p_tty->p_console->original_addr;
	if (nr_tty == 0)
	{
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void select_console(u8 nr_console)
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
	{
		return;
	}
	nr_current_console = nr_console;
	flush(&console_table[nr_console]);
}

PUBLIC u8 is_current_console(struct console* p_console)
{
	return (p_console == &console_table[nr_current_console]);
}

PUBLIC void scroll_screen(struct console* p_console, char direction)
{
	if (direction == SCREEN_UP)
	{
		if (p_console->current_start_addr > p_console->original_addr)
		{
			p_console->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCREEN_DOWN)
	{
		if (p_console->current_start_addr + SCREEN_SIZE < p_console->original_addr + p_console->vmem_limit)
		{
			p_console->current_start_addr += SCREEN_WIDTH;
		}
	}
	flush(p_console);
}

PRIVATE void set_cursor(u32 pos)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (pos >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, pos & 0xFF);
	enable_int();
}

PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}

PRIVATE void flush(struct console* p_console)
{
	if (is_current_console(p_console))
	{
		set_cursor(p_console->cursor);
		set_video_start_addr(p_console->current_start_addr);
	}
}