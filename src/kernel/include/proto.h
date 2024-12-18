#ifndef _CANALOTUS_PROTO_H_
#define _CANALOTUS_PROTO_H_

#include "type.h"
#include "const.h"

/* kernel/i8259.c */
PUBLIC void init_8259A();
PUBLIC void spurious_irq(int irq);

/* kernel/protect.c */
PUBLIC void exception_handler(int vec_no, int error_code, int eip, int cs, int eflags);
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);

/* kernel/kernel.asm */
PUBLIC void restart();

/* kernel/main.c */
PUBLIC int kernel_main();

/* 进程 */
PUBLIC void TestA();
PUBLIC void TestB();

/* lib/kliba.asm */
PUBLIC void disp_str(char* str);
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC void in_byte(u16 port);
PUBLIC void disp_color_str(char* str, u8 color);

/* lib/klib.c */
PUBLIC char* itoa(char* str, int num);
PUBLIC void disp_int(int input);
PUBLIC void delay(int time);

#endif