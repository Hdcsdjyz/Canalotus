/***
 * @file tty.h
 * @author Lhxl
 * @data 2024-12-22
 * @version 1.0
 * @brief 终端
 ***/

#ifndef _CANALOTUS_TTY_H
#define _CANALOTUS_TTY_H

#define TTY_IN_BYTES 256

/***
 * @struct tty
 * @brief 终端
 ***/
struct tty
{
	u16 in_buf[TTY_IN_BYTES];	///< 输入缓冲区
	u16* p_inbuf_head;			///< 指向输入缓冲区下一个空闲位置
	u16* p_inbuf_tail;			///< 指向输入缓冲区应处理的位置
	u8 inbuf_count;				///< 缓冲区大小

	struct console* p_console;	///< 控制台
};

#endif