#include "include/proto.h"
#include "include/global.h"
#include "include/color.h"

PRIVATE void set_cursor(u32 pos);
PRIVATE void set_video_start_addr(u32 addr);

PUBLIC void out_char(struct console* p_console, u8 ch)
{
	u8* p_vmem = (u8*)(VMEM_BASE + p_console->cursor * 2);
	*p_vmem++ = ch;
	*p_vmem++ = DEFAULT_CHAR_COLOR;
	p_console->cursor++;
	set_cursor(p_console->cursor);
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
	else
	{
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}
	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void select_console(u8 nr_console)
{
	if (nr_console < 0 || nr_console >= NR_CONSOLES)
	{
		return;
	}
	nr_current_console = nr_console;
	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}

PRIVATE void set_cursor(u32 pos)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, pos >> 8 & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, pos & 0xFF);
	enable_int();
}

PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, addr >> 8 & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}