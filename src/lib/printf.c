/***
 * @file printf.c
 * @author Lhxl
 * @date 2024-12-28
 * @version build30
 * @brief printf
 ***/

#include "../include/type.h"
#include "../include/sys/proto.h"
#include "../include/stdio.h"

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