#ifndef _CANALOTUS_CONST_H_
#define _CANALOTUS_CONST_H_

/* 函数可见性 */
#define PUBLIC
#define PRIVATE static

/* 变量可见性 */
#define EXTERN extern

/* 描述符 */
#define GDT_SIZE 128
#define IDT_SIZE 256

/* 8259A */
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1

/* 权限级 */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_SYSU 1
#define PRIVILEGE_USER 3

#endif