#include "../include/const.h"
#include "../include/proto.h"

PUBLIC char* itoa(char* str, int num)
{
	char* p = str;
	char ch;
	u8 flag = FALSE;
	*p++ = '0';
	*p++ = 'x';
	if (num == 0)
	{
		*p++ = '0';
	}
	else
	{
		for (int i = 28; i >= 0; i -= 4)
		{
			ch = (num >> i) & 0xF;
			if (flag || (ch > 0))
			{
				flag = TRUE;
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