/***
 * @file color.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 控制台输出颜色
 ***/

#ifndef _CANALOTUS_COLOR_H_
#define _CANALOTUS_COLOR_H_

#define BLACK	0x0
#define WHITE	0x7
#define RED		0x4
#define GREEN	0x2
#define BLUE	0x1
#define FLASH	0x80
#define BRIGHT	0x08

#define MAKE_COLOR(x, y) (x | y)

#endif