/***
 * @file misc.c
 * @author Lhxl
 * @date 2024-12-28
 * @version build30
 * @brief misc
 ***/

#include "../include/sys/const.h"
#include "../include/sys/proto.h"

PUBLIC void assertion_failure(char* exp, char* file, int line)
{
	printf("%c  assert(%s) failed: file: %s, line: %d",
		MAG_CH_ASSERT, exp, file, line);
	spin("assertion_failure");
}

PUBLIC void spin(char* func_name)
{
	printf("\nspinning in %s ...\n",
		func_name);
	while (1)
	{

	}
}