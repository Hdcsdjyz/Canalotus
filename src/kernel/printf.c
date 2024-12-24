#include "include/type.h"
#include "include/proto.h"

int printf(const char* fmt, ...)
{
	int i;
	char buf[256];
	va_list arg = (char*)&fmt + 4;
	i = vsprintf(buf, fmt, arg);
	write(buf, i);
	return i;
}