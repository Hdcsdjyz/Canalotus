/***
 * @file const.h
 * @author Lhxl
 * @date 2024-12-26
 * @version build26
 * @brief 常量定义
 ***/

#ifndef _CANALOTUS_CONST_H_
#define _CANALOTUS_CONST_H_

#include "protect.h"
#include "proc.h"

/* 可见性 */
#define PUBLIC
#define PRIVATE static
#define EXTERN extern

/* 布尔量 */
#define TRUE	1
#define FALSE	0

/* 描述符 */
#define GDT_SIZE 128
#define IDT_SIZE 256

/* 中断 */
/*- 8259A */
#define INT_M_CTL		0x20
#define INT_M_CTLMASK	0x21
#define INT_S_CTL		0xA0
#define INT_S_CTLMASK	0xA1

/*- 硬件计时器 */
#define TIMER0				0x40
#define TIMER_MODE			0x43
#define RATE_GENERATOR		0x34
#define TIMER_FREQUENCY		1193182L
#define HZ					100

/*- 硬件中断 */
#define NR_IRQ			16
#define CLOCK_IRQ		0
#define KEYBOARD_IRQ	1
#define CASCADE_IRQ		2
#define ETHER_IRQ		3
#define SECONDARY_IRQ	3
#define RS232_IRQ		4
#define XT_WINI_IRQ		5
#define FLOPPY_IRQ		6
#define PRINTER_IRQ		7
#define AT_WINI_IRQ		14

/*- 系统调用（软件中断） */
#define NR_SYSCALL 2 // 系统调用数

/* 权限级 */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_SYSU 1
#define PRIVILEGE_USER 3

/* RPL */
#define RPL_KRNL SA_RPL0
#define RPL_SYSU SA_RPL1
#define RPL_USER SA_RPL3

/* 键盘 */
#define KBD_DATA	0x60
#define KBD_CMD		0x64
#define LED_CODE	0xED
#define KBD_ACK		0xFA

/* VGA */
#define CRTC_ADDR_REG	0x3D4
#define CRTC_DATA_REG	0x3D5
#define START_ADDR_H	0xC
#define START_ADDR_L	0xD
#define CURSOR_H		0xE
#define CURSOR_L		0xF
#define VMEM_BASE		0xB8000
#define VMEM_SIZE		0x8000

/* 终端 */
#define NR_CONSOLES 3

/* assert */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char* exp, char* file, int line);
#define assert(exp) if(exp); else assertion_failure(#exp, __FILE__, __LINE__)
#else
#define assert(exp)
#endif

#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

/* 消息 */
/* 消息的目标 */
#define INVALID_DRIVER	-20
#define INTERRUPT		-10
#define SYSPROC_TTY		0
#define SYSPROC_SYS		1
#define SYSPROC_HD		2
#define ANY				(NR_SYSU_PROCS + NR_USER_PROCS + 10)
#define NO_PROC			(NR_SYSU_PROCS + NR_USER_PROCS + 20)
/* 消息类型 */
#define SEND		1
#define RECEIVE		2
#define BOTH		3
/* 进程 */
#define SENDING   0x02
#define RECEIVING 0x04
/***
 * @enum msgtype
 * @brief 消息类型
 ***/
enum msgtype
{
	HARD_INT = 1,
	GET_TICKS,
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};
#define RETVAL		u.m3.m3i1
#define CNT			u.m3.m3i2
#define REQUEST		u.m3.m3i2
#define PROC_NR		u.m3.m3i3
#define DEVICE		u.m3.m3i4
#define POSITION	u.m3.m3l1
#define BUF			u.m3.m3p2
#define STR_DEFAULT_LEN 1024

/* 硬盘 */
#define DIOCTL_GET_GEO		1
#define SECTOR_SIZE			512
#define SECTOR_BITS			(SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT	9
/* 硬件类型 */
#define NO_DEV			0
#define DEV_FLOPPY		1
#define DEV_CDROM		2
#define DEV_HD			3
#define DEV_CHAR_TTY	4
#define DEV_SCSI		5

#define MAJOR_SHIFT 8
#define MAKE_DEV(a,b) ((a << MAJOR_SHIFT) | b)
#define MAJOR(x) ((x >> MAJOR_SHIFT) & 0xFF)
#define MINOR(x) (x & 0xFF)
/* 硬盘结构 */
#define MINOR_hd1a		0x10
#define MINOR_hd2a		0x20
#define MINOR_hd2b		0x21
#define MINOR_hd3a		0x30
#define MINOR_hd4a		0x40
/* 目录结构 */
#define ROOT_DEV		MAKE_DEV(DEV_HD, MINOR_BOOT)
#define INVALID_INODE	0
#define ROOT_INODE		1
/* 硬盘 */
#define MAX_DRIVERS			2	// 最大硬盘数
#define NR_PART_PER_DRIVE	4	// 每个硬盘最大主分区数
#define NR_SUB_PER_PART		16	// 每个扩展分区最大逻辑分区数
#define NR_SUB_PER_DRIVE	(NR_SUB_PER_PART * NR_PART_PER_DRIVE)	// 每个硬盘最大逻辑分区数
#define NR_PRIM_PER_DRIVE	(NR_PART_PER_DRIVE + 1)					//
#define MAX_PRIM			(MAX_DRIVERS * NR_PRIM_PER_DRIVE - 1)	// 最大总分区数
#define MAX_SUBPARTITIONS	(NR_SUB_PER_DRIVE * MAX_DRIVERS)		// 最大逻辑分区数
/* 分区类型 */
#define P_PRIMARY		0
#define P_EXTENDED		1
/* 分区系统标识 */
#define CANALOTUS_PART	0x99
#define NO_PART			0x00
#define EXT_PART		0x05
/* 各结构数量 */
#define NR_FILES		64
#define NR_FILE_DESC	64
#define NR_INODE		64
#define NR_SUPER		8

#define I_TYPE_MASK			0170000
#define I_REGULAR			0100000
#define I_BLOCK_SPECIAL		0060000
#define I_DIRECTORY			0040000
#define I_CHAR_SPECIAL		0020000
#define I_NAMED_PIPE		0010000

#define IS_SPECIAL(m)	((((m) & I_TYPE_MASK) == I_BLOCK_SPECIAL) || (((m) &I_TYPE_MASK) == I_CHAR_SPECIAL))

#define NR_DEFAULT_FILE_SECTS	2048

#endif