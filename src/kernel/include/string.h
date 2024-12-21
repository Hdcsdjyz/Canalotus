/***
 * @file string.h
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief string.h
 ***/

#ifndef _CANALOTUS_STRING_H_
#define _CANALOTUS_STRING_H_

#include "const.h"

/***
 * @param[out] p_dst (void*)目标指针
 * @param[in] p_src (void*)源指针
 * @param size (int)大小
 ***/
PUBLIC void* memcpy(void* p_dst, void* p_src, int size);

/***
 * @param[out] p_dst (void*)目标指针
 * @param[in] ch (char)内容
 * @param[in] size (int)大小
 ***/
PUBLIC void memset(void* p_dst, char ch, int size);

/***
 * @param[out] p_dst (char*)目标指针
 * @param[in] p_src (char*)源指针
 * @return (char*)目标指针
 ***/
PUBLIC char* strcpy(char* p_dst, char* p_src);

#endif