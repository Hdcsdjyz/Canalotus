#ifndef _CANALOTUS_TYPE_H
#define _CANALOTUS_TYPE_H

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

typedef void (*int_handler) ();
typedef void (*task_f) ();
typedef void (*irq_handler) (int irq);

#endif