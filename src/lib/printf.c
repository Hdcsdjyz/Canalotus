/***
 * @file printf.c
 * @author Lhxl
 * @date 2025-1-13
 * @version build30
 * @brief printf
 ***/

#include "../include/sys/type.h"
#include "../include/sys/proto.h"

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