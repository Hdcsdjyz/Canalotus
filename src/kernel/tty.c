#include "include/const.h"
#include "include/proto.h"
#include "include/keyboard.h"

PUBLIC void sys_tty()
{
	while (1)
	{
		keyboard_read();
	}
}

PUBLIC void in_process(u16 key)
{
	u8 output[2] = {'\0', '\0'};
	if (!(key & FLAG_EXT))
	{
		output[0] = key & 0xFF;
		disp_str(output);
	}
}