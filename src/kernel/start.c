/***
 * @file start.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 初始化内核
 ***/

#include "include/type.h"
#include "include/const.h"
#include "include/protect.h"
#include "include/proto.h"
#include "include/string.h"
#include "include/global.h"

PUBLIC void cstart()
{
    memcpy(&gdt, (void*)(*(u32*)&gdt_ptr[2]), *(u16*)&gdt_ptr[0] + 1);
    u16* p_gdt_limit = (u16*)&gdt_ptr[0];
    u32* p_gdt_base = (u32*)&gdt_ptr[2];
    *p_gdt_limit = GDT_SIZE * sizeof(struct descriptor) - 1;
    *p_gdt_base = (u32)&gdt;

    u16* p_idt_limit = (u16*)&idt_ptr[0];
    u32* p_idt_base = (u32*)&idt_ptr[2];
    *p_idt_limit = IDT_SIZE * sizeof(struct gate) - 1;
    *p_idt_base = (u32)&idt;

    init_prot();
}