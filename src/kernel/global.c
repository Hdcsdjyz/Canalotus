/***
 * @file global.c
 * @author Lhxl
 * @date 2025-3-27
 * @version build31
 * @brief 全局数组定义
 ***/

#define GLOBAL_VAR_HERE

#include "../include/sys/global.h"
#include "../include/sys/proto.h"
#include "../include/sys/type.h"
#include "../include/sys/proto.h"

/* 进程 */
PUBLIC struct process proc_table[NR_SYSU_PROCS + NR_USER_PROCS];
PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC struct task sysu_proc_table[NR_SYSU_PROCS] = {
	{sys_tty, STACK_SIZE_SYS_TTY,"tty"},
	{sys_sys, STACK_SIZE_SYS_SYS,"sys"},
	{sys_hd, STACK_SIZE_SYS_HD, "hd"},
	{sys_fs, STACK_SIZE_SYS_FS, "fs"}
};

PUBLIC struct task user_proc_table[NR_USER_PROCS] = {
	{TestA, STACK_SIZE_TESTA, "TestA"},
	{TestB, STACK_SIZE_TESTB, "TestB"},
	{TestC, STACK_SIZE_TESTC, "TestC"}

};

/* 中断 */
PUBLIC irq_handler irq_table[NR_IRQ];
PUBLIC syscall syscall_table[NR_SYSCALL] = {
	syscall_printx,
	syscall_sendrec
};

/* 终端 */
PUBLIC struct tty tty_table[NR_CONSOLES];
PUBLIC struct console console_table[NR_CONSOLES];

/***
 * @struct dev_drv_map
 * @brief 驱动程序表
 ***/
struct dev_drv_map dd_map[] = {
	{INVALID_DRIVER},	///< 未使用
	{INVALID_DRIVER},	///<
	{INVALID_DRIVER},	///<
	{SYSPROC_HD},		///< 硬盘
	{SYSPROC_TTY},				///< 终端
	{INVALID_DRIVER}		///<
};

/* 文件系统 */
PUBLIC u8* fsbuf = (u8*)0x600000;
PUBLIC const int FSBUF_SIZE = 0x100000;