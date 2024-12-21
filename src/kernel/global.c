/***
 * @file global.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 全局数组定义
 ***/

#define GLOBAL_VAR_HERE

#include "include/type.h"
#include "include/global.h"
#include "include/proto.h"
#include "include/const.h"
#include "include/protect.h"
#include "include/proc.h"

/* 进程 */
PUBLIC struct process proc_table[NR_TASKS];
PUBLIC u8 task_stack[STACK_SIZE_TOTAL];
PUBLIC struct task task_table[NR_TASKS] = {
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"},
	{sys_tty, STACK_SIZE_SYS_TTY,"sys_tty"}
};

/* 中断 */
PUBLIC irq_handler irq_table[NR_IRQ];
PUBLIC syscall syscall_table[NR_SYSCALL] = {
	syscall_get_ticks
};