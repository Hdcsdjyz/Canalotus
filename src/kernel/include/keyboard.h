/***
 * @file keyboard.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 键盘输入相关定义
 ***/

#ifndef _CANALOTUS_KEYBOARD_H_
#define _CANALOTUS_KEYBOARD_H_

#define KBD_INBYTES		32
#define MAP_COLS		3
#define NR_SCAN_CODES	0x80

#define FLAG_BREAK		0x0080	///< 标签
#define FLAG_EXT		0x0100	///< 标签
#define FLAG_LSHIFT		0x0200	///< 标签
#define FLAG_RSHIFT		0x0400	///< 标签
#define FLAG_LCTRL		0x0800	///< 标签
#define FLAG_RCTRL		0x1000	///< 标签
#define FLAG_LALT		0x2000	///< 标签
#define FLAG_RALT		0x4000	///< 标签
#define FLAG_PAD		0x8000	///< 标签

#define MASK_RAW		0x01FF

/* special keys */
#define ESC				(0x01 + FLAG_EXT)	///< 特殊键
#define TAB				(0x02 + FLAG_EXT)	///< 特殊键
#define ENTER			(0x03 + FLAG_EXT)	///< 特殊键
#define BACKSPACE		(0x04 + FLAG_EXT)	///< 特殊键
#define LGUI			(0x05 + FLAG_EXT)	///< 特殊键
#define RGUI			(0x06 + FLAG_EXT)	///< 特殊键
#define APPS			(0x07 + FLAG_EXT)	///< 特殊键

/* shift ctrl alt */
#define LSHIFT			(0x08 + FLAG_EXT)	///< shift
#define RSHIFT			(0x09 + FLAG_EXT)	///< shift
#define LCTRL			(0x0A + FLAG_EXT)	///< ctrl
#define RCTRL			(0x0B + FLAG_EXT)	///< ctrl
#define LALT			(0x0C + FLAG_EXT)	///< alt
#define RALT			(0x0D + FLAG_EXT)	///< alt

/* lock keys */
#define CAPS_LOCK		(0x0E + FLAG_EXT)	///< 锁定键
#define NUM_LOCK		(0x0F + FLAG_EXT)	///< 锁定键
#define SCROLL_LOCK		(0x10 + FLAG_EXT)	///< 锁定键

/* function keys */
#define F1				(0x11 + FLAG_EXT)	///< 功能键
#define F2				(0x12 + FLAG_EXT)	///< 功能键
#define F3				(0x13 + FLAG_EXT)	///< 功能键
#define F4				(0x14 + FLAG_EXT)	///< 功能键
#define F5				(0x15 + FLAG_EXT)	///< 功能键
#define F6				(0x16 + FLAG_EXT)	///< 功能键
#define F7				(0x17 + FLAG_EXT)	///< 功能键
#define F8				(0x18 + FLAG_EXT)	///< 功能键
#define F9				(0x19 + FLAG_EXT)	///< 功能键
#define F10				(0x1A + FLAG_EXT)	///< 功能键
#define F11				(0x1B + FLAG_EXT)	///< 功能键
#define F12				(0x1C + FLAG_EXT)	///< 功能键

/* control keys */
#define PRINTSCREEN		(0x1D + FLAG_EXT)	///< 控制键
#define PAUSEBREAK		(0x1E + FLAG_EXT)	///< 控制键
#define INSERT			(0x1F + FLAG_EXT)	///< 控制键
#define DELETE			(0x20 + FLAG_EXT)	///< 控制键
#define HOME			(0x21 + FLAG_EXT)	///< 控制键
#define END				(0x22 + FLAG_EXT)	///< 控制键
#define PAGEUP			(0x23 + FLAG_EXT)	///< 控制键
#define PAGEDOWN		(0x24 + FLAG_EXT)	///< 控制键
#define UP				(0x25 + FLAG_EXT)	///< 控制键
#define DOWN			(0x26 + FLAG_EXT)	///< 控制键
#define LEFT			(0x27 + FLAG_EXT)	///< 控制键
#define RIGHT			(0x28 + FLAG_EXT)	///< 控制键

/* ACPI keys */
#define POWER			(0x29 + FLAG_EXT)	///< ACHI键
#define SLEEP			(0x2A + FLAG_EXT)	///< ACHI键
#define WAKE			(0x2B + FLAG_EXT)	///< ACHI键

/* num pad */
#define PAD_SLASH		(0x2C + FLAG_EXT)	///< 小键盘键
#define PAD_STAR		(0x2D + FLAG_EXT)	///< 小键盘键
#define PAD_MINUS		(0x2E + FLAG_EXT)	///< 小键盘键
#define PAD_PLUS		(0x2F + FLAG_EXT)	///< 小键盘键
#define PAD_ENTER		(0x30 + FLAG_EXT)	///< 小键盘键
#define PAD_DOT			(0x31 + FLAG_EXT)	///< 小键盘键
#define PAD_0			(0x32 + FLAG_EXT)	///< 小键盘键
#define PAD_1			(0x33 + FLAG_EXT)	///< 小键盘键
#define PAD_2			(0x34 + FLAG_EXT)	///< 小键盘键
#define PAD_3			(0x35 + FLAG_EXT)	///< 小键盘键
#define PAD_4			(0x36 + FLAG_EXT)	///< 小键盘键
#define PAD_5			(0x37 + FLAG_EXT)	///< 小键盘键
#define PAD_6			(0x38 + FLAG_EXT)	///< 小键盘键
#define PAD_7			(0x39 + FLAG_EXT)	///< 小键盘键
#define PAD_8			(0x3A + FLAG_EXT)	///< 小键盘键
#define PAD_9			(0x3B + FLAG_EXT)	///< 小键盘键
#define PAD_UP			PAD_8				///< 小键盘键
#define PAD_DOWN		PAD_2				///< 小键盘键
#define PAD_LEFT		PAD_4				///< 小键盘键
#define PAD_RIGHT		PAD_6				///< 小键盘键
#define PAD_HOME		PAD_7				///< 小键盘键
#define PAD_END			PAD_1				///< 小键盘键
#define PAD_PAGEUP		PAD_9				///< 小键盘键
#define PAD_PAGEDOWN	PAD_3				///< 小键盘键
#define PAD_INS			PAD_0				///< 小键盘键
#define PAD_MID			PAD_5				///< 小键盘键
#define PAD_DEL			PAD_DOT				///< 小键盘键

/***
 * @struct keyboard
 * @brief 键盘
 ***/
struct keyboard
{
	u8* p_head;				///< 指向buf中下一个空闲位置
	u8* p_tail;				///< 指向键盘程序应处理的字节
	int count;				///< 缓冲区中的字节数
	u8 buf[KBD_INBYTES];	///< 缓冲区
};

#endif