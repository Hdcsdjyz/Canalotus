/***
 * @file color.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 控制台输出颜色
 ***/

#ifndef _CANALOTUS_COLOR_H_
#define _CANALOTUS_COLOR_H_

#define BLACK	0x0		///< 0000 0000
#define WHITE	0x7		///< 0000 0111
#define RED		0x4		///< 0000 0100
#define GREEN	0x2		///< 0000 0010
#define BLUE	0x1		///< 0000 0001
#define FLASH	0x80	///< 1000 0000
#define BRIGHT	0x08	///< 0000 1000

#define DEFAULT_CHAR_COLOR (MAKE_COLOR(WHITE, BRIGHT))

#define MAKE_COLOR(x, y) (x | y)

#endif