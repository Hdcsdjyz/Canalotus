#include "include/type.h"
#include "include/const.h"
#include "include/protect.h"

PUBLIC void* memcpy(void* pDst, void* pSrc, int Size);

PUBLIC u8 gdt_ptr[6];
PUBLIC struct descriptor gdt[GDT_SIZE];

PUBLIC void cstart()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
			 "-----\"cstart()\"-----");
	memcpy(&gdt, (void*)*(u32*)&gdt_ptr[2], *(u16*)&gdt_ptr[0] + 1);
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(struct descriptor) - 1;
	*p_gdt_base = (u32)&gdt;
}