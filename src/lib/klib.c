#include "../kernel/include/const.h"
#include "../kernel/include/proto.h"

PUBLIC char* itoa(char* str, int num)
{
	char* p = str;
	char ch;
	int i = 0;
	int flag = 0;
	*p++ = '0';
	*p++ = 'x';
	if (num == 0)
	{
		*p++ = '0';
	}
	else
	{
		for (i = 28; i >= 0; i -= 4)
		{
			ch = num >> i & 0xF;
			if (flag || ch > 0)
			{
				flag = 1;
				ch += '0';
				if (ch > '9')
				{
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}
	*p = 0;
	return str;
}

PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}

PUBLIC void delay(int time)
{
	int i, j, k;
	for (k=0; k < time; k++)
	{
		for (i=0; i < 10; i++)
		{
			for (j=0; j < 10000; j++)
			{

			}
		}
	}
}