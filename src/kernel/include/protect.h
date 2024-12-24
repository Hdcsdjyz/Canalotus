/***
 * @file protect.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 保护模式相关定义
 ***/

#ifndef _CANALOTUS_PROTECT_H_
#define _CANALOTUS_PROTECT_H_

#include "type.h"

/***
 * @struct descriptor
 * @brief 段描述符
 ***/
struct descriptor
{
	u16 limit_low;			///< 段界限低16位
	u16 base_low;			///< 段基址低16位
	u8 base_mid;			///< 段基址中8位
	u8 attr1;				///< 段属性
	u8 limit_high_attr2;	///< 段界限高4位及属性
	u8 base_high;			///< 段基址高8位
};

/***
 * @struct gate
 * @brief 门
 ***/
struct gate
{
	u16 offset_low;		///< 偏移量低16位
	u16 selector;		///< 段选择子
	u8 dcount;			///<
	u8 attr;			///< 属性
	u16 offset_high;	///< 偏移量高16位
};

/* TSS */
struct tss
{
	u32 backlink;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iomap;
};

/* 中断向量 */
/*- 异常 */
#define INT_VECTOR_DIVIDE 0x0			///< 异常
#define INT_VECTOR_DEBUG 0x1			///< 异常
#define INT_VECTOR_NMI 0x2				///< 异常
#define INT_VECTOR_BREAKPOINT 0x3		///< 异常
#define INT_VECTOR_OVERFLOW 0x4			///< 异常
#define INT_VECTOR_BOUNDS 0x5			///< 异常
#define INT_VECTOR_INVAL_OP 0x6			///< 异常
#define INT_VECTOR_COPROC_NOT 0x7		///< 异常
#define INT_VECTOR_DOUBLE_FAULT 0x8		///< 异常
#define INT_VECTOR_COPROC_SEG 0x9		///< 异常
#define INT_VECTOR_INVAL_TSS 0xA		///< 异常
#define INT_VECTOR_SEG_NOT 0xB			///< 异常
#define INT_VECTOR_STACK_FAULT 0xC		///< 异常
#define INT_VECTOR_PROTECTION 0xD		///< 异常
#define INT_VECTOR_PAGE_FAULT 0xE		///< 异常
#define INT_VECTOR_COPROC_ERR 0x10		///< 异常

/*- 硬件中断 */
#define INT_VECTOR_IRQ0 0x20	///< 硬件中断
#define INT_VECTOR_IRQ8 0x28	///< 硬件中断

/*- 系统调用（软件中断） */
#define INT_VECTOR_SYSCALL 0x30	///< 系统调用

/* 选择子 */
#define SELECTOR_DUMMY 0
#define SELECTOR_FLAT_C 0x08
#define SELECTOR_FLAT_RW 0x10
#define SELECTOR_VIDEO 0x1B
#define SELECTOR_TSS 0x20
#define SELECTOR_LDT_FIRST 0x28

#define SELECTOR_KERNEL_CS SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS SELECTOR_VIDEO

/* 选择子属性 */
#define SA_RPL_MASK 0xFFFC
#define SA_RPL0		0
#define SA_RPL1		1
#define SA_RPL2		2
#define SA_RPL3		3
#define SA_TI_MASK  0xFFFB
#define SA_TIG	    0
#define SA_TIL	    4

/* 段属性 */
#define DA_32           0x4000  ///< 32位段
#define DA_LIMIT_4K     0x8000  ///< 段界限颗粒度为4KB

#define DA_DPL0         0x00    ///< DPL = 0
#define DA_DPL1         0x20    ///< DPL = 1
#define DA_DPL2         0x40    ///< DPL = 2
#define DA_DPL3         0x60    ///< DPL = 3

#define DA_DR           0x90    ///< 存在、只读、数据
#define DA_DRW          0x92    ///< 存在、可读写、数据
#define DA_DRWA         0x93    ///< 存在、已访问、可读写、数据
#define DA_C            0x98    ///< 存在、只执行、代码
#define DA_CR           0x9A    ///< 存在、可执行、可读、代码
#define DA_CCO          0x9C    ///< 存在、只执行、一致、代码
#define DA_CCOR         0x9E    ///< 存在、可执行、可读、一致、代码

#define DA_LDT          0x82    ///<
#define DA_TaskGate     0x85    ///< 任务门
#define DA_386TSS       0x89    ///< 状态段
#define DA_386CGate     0x8C    ///< 调用门
#define DA_386IGate     0x8E    ///< 中断门
#define DA_386TGate     0x8F    ///< 陷阱门

/* GDT */
#define INDEX_DUMMY			0
#define INDEX_FLAT_C		1
#define INDEX_FLAT_RW		2
#define INDEX_VIDEO			3
#define INDEX_TSS			4
#define INDEX_LDT_FIRST		5

/* 进程 */
#define LDT_SIZE 2

/* 由线性地址求绝对地址 */
#define vir2phys(seg_base, vir) (u32)((u32)(seg_base) + (u32)(vir))

#endif