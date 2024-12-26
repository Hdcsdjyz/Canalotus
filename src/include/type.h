/***
 * @file type.h
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief 类型别名
 ***/

#ifndef _CANALOTUS_TYPE_H
#define _CANALOTUS_TYPE_H

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef void* syscall;
typedef char* va_list;

typedef void (*int_handler) ();
typedef void (*task_f) ();
typedef void (*irq_handler) (int irq);

struct msg1
{
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};

struct msg2
{
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};

struct msg3
{
	int m3i1;
	int m3i2;
	int m3i3;
	int m3i4;
	u64 m3l1;
	u64 m3l2;
	void* m3p1;
	void* m3p2;
};

struct message
{
	int source;
	int type;
	union
	{
		struct msg1 m1;
		struct msg2 m2;
		struct msg3 m3;
	}u;
};

#endif