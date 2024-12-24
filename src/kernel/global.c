/***
 * @file global.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 全局数组定义
 ***/

#define GLOBAL_VAR_HERE

#include "include/global.h"
#include "include/proto.h"

/* 进程 */
PUBLIC struct process proc_table[NR_SYSU_PROCS + NR_USER_PROCS];
PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC struct task sysu_proc_table[NR_SYSU_PROCS] = {
	{sys_tty, STACK_SIZE_SYS_TTY,"tty"},
};

PUBLIC struct task user_proc_table[NR_USER_PROCS] = {
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"}

};

/* 中断 */
PUBLIC irq_handler irq_table[NR_IRQ];
PUBLIC syscall syscall_table[NR_SYSCALL] = {
	syscall_get_ticks
};

/* 终端 */
PUBLIC struct tty tty_table[NR_CONSOLES];
PUBLIC struct console console_table[NR_CONSOLES];
