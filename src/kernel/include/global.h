#ifndef _CANALOTUS_GLOBAL_H
#define _CANALOTUS_GLOBAL_H

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"

#ifdef GLOBAL_VAR_HERE
#undef EXTERN
#define EXTERN
#endif

EXTERN int disp_pos; // 当前显示的位置

/* GDT */
EXTERN u8 gdt_ptr[6];
EXTERN struct descriptor gdt[GDT_SIZE];

/* IDT */
EXTERN u8 idt_ptr[6];
EXTERN struct gate idt[IDT_SIZE];

/* 进程 */
extern struct process proc_table[];
extern u8 task_stack[];
extern struct task task_table[];
EXTERN struct tss tss;
EXTERN struct process* p_proc_ready;

/* 中断 */
EXTERN u32 k_reenter;

#endif