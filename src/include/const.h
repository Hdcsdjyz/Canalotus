/***
 * @file const.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 常量定义
 ***/

#ifndef _CANALOTUS_CONST_H_
#define _CANALOTUS_CONST_H_

#include "protect.h"

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
void assertion_failure(char* exp, char* file, char* base_file, int line);
#define assert(exp) if(exp) else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

#endif