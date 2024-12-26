/***
 * @file printf.c
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief printf
 ***/

#include "../include/type.h"
#include "../include/proto.h"

int printf(const char* fmt, ...)
{
	int i;
	char buf[256];
	va_list arg = (char*)&fmt + 4;
	i = vsprintf(buf, fmt, arg);
	buf[i] = 0;
	printx(buf);
	return i;
}