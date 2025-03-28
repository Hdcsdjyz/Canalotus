/***
 * @file stdio.h
 * @author Lhxl
 * @data 2025-1-13
 * @version build30
 * @brief stdio
 ***/

#include "sys/type.h"

int vsprintf(char* buf, const char* format, va_list args);
int sprintf(char* buf, const char* fmt, ...);
int printf(const char* format, ...);