#ifndef _CANALOTUS_PROC_H_
#define _CANALOTUS_PROC_H_

#include "type.h"
#include "protect.h"

/* 进程堆栈 */
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

/* 进程信息 */
struct process
{
	struct stackframe regs;
	u16 ldt_sel;
	struct descriptor ldts[LDT_SIZE];
	u32 pid;
	char p_name[16];
};

struct task
{
	task_f initial_eip;
	int stacksize;
	char name[32];
};

#define NR_TASKS 3 // 进程数

#define STACK_SIZE_TESTA 0x8000 // 进程TestA堆栈大小
#define STACK_SIZE_TESTB 0x8000 // 进程TestB堆栈大小
#define STACK_SIZE_TESTC 0x8000 // 进程TestC堆栈大小
#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA\
						+ STACK_SIZE_TESTB\
						+ STACK_SIZE_TESTC) // 进程堆栈总大小

#endif