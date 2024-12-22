/***
 * @file console.h
 * @author Lhxl
 * @data 2024-12-22
 * @version 1.0
 * @brief 控制台
 ***/

#ifndef _CANALOTUS_CONSOLE_H_
#define _CANALOTUS_CONSOLE_H_

#include "type.h"

/***
 * @struct console
 * @brief 控制台
 ***/
struct console
{
	u32 current_start_addr;		///< 当前显示位置
	u32 original_addr;			///< 当前显存位置
    u32 vmem_limit;				///< 当前显存大小
    u32 cursor;					///< 当前光标位置
};

#endif