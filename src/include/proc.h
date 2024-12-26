/***
 * @file proc.h
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief 进程相关定义
 ***/

#ifndef _CANALOTUS_PROC_H_
#define _CANALOTUS_PROC_H_

#include "type.h"
#include "protect.h"

/***
 * @struct stackframe
 * @brief 进程的寄存器帧
 ***/
struct stackframe
{
	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
    u32 kernel_esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 ret_addr;
	u32 eip;
	u32 cs;
	u32 eflags;
	u32 esp;
	u32 ss;
};

/***
 * @struct process
 * @brief 进程信息
 ***/
struct process
{
	struct stackframe regs;				///< 寄存器栈
	u16 ldt_sel;						///< idt选择子
	struct descriptor ldts[LDT_SIZE];   ///< 代码段和数据段idt
	int ticks;							///< 当前运行占用帧数
	int priority;						///< 优先级
	u32 pid;							///< pid
	char name[16];						///< 进程名称
	int p_flags;						///< 运行标志
	struct message* p_msg;				///< 信息
	int p_recvfrom;						///< 当前发送信息至此进程的进程
	int p_sendto;						///< 当前将要发送消息至的进程
	int has_int_msg;					///< 是否中断
	struct process* q_sending;			///< 等待发送信息至此进程的进程
	struct process* next_sending;		///< 下一个将要发送消息至的进程
	u8 nr_tty;							///< 控制台号
};

/***
 * @struct task
 * @brief 进程
 ***/
struct task
{
	task_f initial_eip;		///< 进程的起始地址
	int stacksize;			///< 进程的堆栈大小
	char name[32];			///< 进程的名称
};

#define NR_SYSU_PROCS 4 ///< 系统进程数
#define NR_USER_PROCS 3 ///< 用户进程数

#define FIRST_PROC	proc_table[0]
#define LAST_PROC	proc_table[NR_SYSU_PROCS + NR_USER_PROCS - 1]

#define STACK_SIZE_TESTA 0x8000 ///< 进程TestA堆栈大小
#define STACK_SIZE_TESTB 0x8000 ///< 进程TestB堆栈大小
#define STACK_SIZE_TESTC 0x8000 ///< 进程TestC堆栈大小

#define STACK_SIZE_SYS_TTY 0x8000
#define STACK_SIZE_SYS_SYS 0x8000
#define STACK_SIZE_SYS_HD 0x8000
#define STACK_SIZE_SYS_FS 0x8000

#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA\
						+ STACK_SIZE_TESTB\
						+ STACK_SIZE_TESTC\
						+ STACK_SIZE_SYS_TTY\
						+ STACK_SIZE_SYS_SYS\
						+ STACK_SIZE_SYS_HD\
						+ STACK_SIZE_SYS_FS) ///< 进程堆栈总大小

#define proc2pid(x) (x - proc_table)

#endif