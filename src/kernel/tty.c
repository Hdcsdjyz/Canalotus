#include "include/const.h"
#include "include/proto.h"

PUBLIC void sys_tty()
{
	while (1)
	{
		keyboard_read();
	}
}