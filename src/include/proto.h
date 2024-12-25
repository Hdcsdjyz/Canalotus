/***
 * @file proto.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 全局函数声明
 ***/

#ifndef _CANALOTUS_PROTO_H_
#define _CANALOTUS_PROTO_H_

#include "type.h"
#include "const.h"
#include "tty.h"
#include "proc.h"

/* kernel/i8259.c */
PUBLIC void init_8259A();
PUBLIC void spurious_irq(int irq);
PUBLIC void put_irq_handler(int irq, irq_handler handler);

/* kernel/protect.c */
PUBLIC void exception_handler(int vec_no, int error_code, int eip, int cs, int eflags);
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);

/* kernel/kernel.asm */
PUBLIC void restart();

/* kernel/main.c */
PUBLIC int kernel_main();

/*- 进程 */
void TestA();
void TestB();
void TestC();

/* kernel/clock.c */
PUBLIC void init_clock();
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int ms);

/* kernel/syscall.asm */
PUBLIC void _syscall();
PUBLIC int get_ticks();
PUBLIC void write(char* buf, int len);

/* kernel/proc.c */
PUBLIC int syscall_get_ticks();
PUBLIC void schedule();
PUBLIC int sys_sendrec(int function, int src_dst, struct message* m, struct process* p);

/* kernel/keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_handler(int irq);
PUBLIC void keyboard_read(struct tty* p_tty);

/* kernel/tty.c */
PUBLIC void sys_tty();
PUBLIC void in_process(struct tty* p_tty, u16 key);
PUBLIC void tty_write(struct tty* p_tty, char* buf, int len);
PUBLIC int syscall_write(char* buf, int len, struct process* p_proc);

/* kernel/console.c */
PUBLIC void out_char(struct console* p_console, u8 ch);
PUBLIC void init_screen(struct tty* p_tty);
PUBLIC void select_console(u8 nr_console);
PUBLIC u8 is_current_console(struct console* p_console);
PUBLIC void scroll_screen(struct console* p_console, char direction);

/* kernel/vsprintf.c */
int vsprintf(char* buf, const char* format, va_list args);

/* kernel/printf.c */
int printf(const char* format, ...);

/* lib/kliba.asm */
PUBLIC void disp_str(char* str);
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_color_str(char* str, u8 color);
PUBLIC void disable_irq(int irq);
PUBLIC void enable_irq(int irq);
PUBLIC void disable_int();
PUBLIC void enable_int();

/* lib/klib.c */
PUBLIC char* itoa(char* str, int num);
PUBLIC void disp_int(int input);

#endif