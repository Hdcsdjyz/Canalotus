#include "include/const.h"
#include "include/proto.h"
#include "include/global.h"

PUBLIC int syscall_get_ticks()
{
	return ticks;
}