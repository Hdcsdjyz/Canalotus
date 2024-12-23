/***
 * @file global.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 全局变量定义
 ***/

#ifndef _CANALOTUS_GLOBAL_H
#define _CANALOTUS_GLOBAL_H

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "console.h"
#include "tty.h"

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
extern struct task sysu_proc_table[];
extern struct task user_proc_table[];

EXTERN struct tss tss;
EXTERN struct process* p_proc_ready;

/* 中断 */
EXTERN u32 k_reenter;
extern irq_handler irq_table[];

/* sys */
EXTERN int ticks;

/* 终端 */
extern struct tty tty_table[];
extern struct console console_table[];
EXTERN u8 nr_current_console;

#endif