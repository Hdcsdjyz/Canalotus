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
PUBLIC struct task task_table[NR_TASKS] = {{TestA, STACK_SIZE_TESTA, "TestA"},
										   {TestB, STACK_SIZE_TESTB, "TestB"}};

/* 中断 */
PUBLIC irq_handler irq_table[NR_IRQ];