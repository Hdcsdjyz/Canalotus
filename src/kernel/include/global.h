#ifndef _CANALOTUS_GLOBAL_H
#define _CANALOTUS_GLOBAL_H

#include "type.h"
#include "const.h"
#include "protect.h"

#ifdef GLOBAL_VAR_HERE
#undef EXTERN
#define EXTERN
#endif

EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6];
EXTERN struct descriptor gdt[GDT_SIZE];
EXTERN u8 idt_ptr[6];
EXTERN struct gate idt[IDT_SIZE];

#endif