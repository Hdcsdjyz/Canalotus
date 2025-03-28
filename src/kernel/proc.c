/***
 * @file proc.c
 * @author Lhxl
 * @date 2024-12-28
 * @version build30
 * @brief 进程调度
 ***/

#include "../include/sys/config.h"
#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/global.h"
#include "../include/string.h"
#include "../include/sys/color.h"
#include "../include/stdio.h"
#include "../include/assert.h"

PRIVATE void block(struct process* p_proc);
PRIVATE void unblock(struct process* p_proc);
PRIVATE int msg_send(struct process* cnt, int dst, struct message* msg);
PRIVATE int msg_receive(struct process* cnt, int src, struct message* msg);
PRIVATE int deadlock(int src, int dst);

PUBLIC void schedule()
{
	struct process* p;
	int greatest_ticks = 0;
	while (!greatest_ticks)
	{
		for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
		{
			if (p->p_flags == 0)
			{
				if (p->ticks > greatest_ticks)
				{
					greatest_ticks = p->ticks;
					p_proc_ready = p;
				}
			}
		}
		if (!greatest_ticks)
		{
			for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
			{
				if (p->p_flags == 0)
				{
					p->ticks = p->priority;
				}
			}
		}
	}
}

/***
 * @param function 功能
 * @param src_dst 源或目标
 * @param msg 消息
 * @param p_proc 进程
 * @return
 ***/
PUBLIC int syscall_sendrec(int function, int src_dst, struct message* msg, struct process* p_proc)
{
	assert(k_reenter == 0);
	assert((src_dst >= 0 && src_dst < NR_SYSU_PROCS + NR_USER_PROCS) || src_dst == ANY || src_dst == INTERRUPT);
	int ret = 0;
	int caller = proc2pid(p_proc);
	struct message* mla = va2la(caller, msg);
	mla->source = caller;
	assert(mla->source != src_dst);
	if (function == SEND)
	{
		ret = msg_send(p_proc, src_dst, msg);
		if (ret != 0)
		{
			return ret;
		}
	}
	else if (function == RECEIVE)
	{
		ret = msg_receive(p_proc, src_dst, msg);
		if (ret != 0)
		{
			return ret;
		}
	}
	else
	{
		panic("syscall_sendrec: invvavlid function: %d",
			function);
	}
	return 0;
}

PUBLIC void* va2la(int pid, void* va)
{
	struct process* p = &proc_table[pid];
	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;
	if (pid < NR_SYSU_PROCS + NR_USER_PROCS)
	{
		assert(la == (u32)va);
	}
	return (void*)la;
}

PUBLIC int ldt_seg_linear(struct process* p, int idx)
{
	struct descriptor* d = &p->ldts[idx];
	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

PUBLIC void reset_msg(struct message* p)
{
	memset(p, 0, sizeof(struct message));
}

PUBLIC int send_recv(int function, int src_dst, struct message* msg)
{
	int ret = 0;
	if (function == RECEIVE)
	{
		memset(msg, 0, sizeof(struct message));
	}
	switch (function)
	{
	case BOTH:
		ret = sendrec(SEND, src_dst, msg);
		if (ret == 0)
		{
			ret = sendrec(RECEIVE, src_dst, msg);
		}
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(function, src_dst, msg);
		break;
	default:
		assert((function == BOTH) || (function == SEND) || (function == RECEIVE));
		break;
	}
	return ret;
}

PUBLIC void dump_proc(struct process* p)
{
	char info[STR_DEFAULT_LEN];
	int text_color = MAKE_COLOR(GREEN, RED);
	int dump_len = sizeof(struct process);
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, 0);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, 0);
	sprintf(info, "byte dump of proc_table[%d]:\n", p - proc_table);
	disp_color_str(info, text_color);
	for (int i = 0; i < dump_len; i++)
	{
		sprintf(info, "%x.", ((unsigned char *)p)[i]);
		disp_color_str(info, text_color);
	}
	disp_color_str("\n\n", text_color);
	sprintf(info, "ANY: 0x%x.\n", ANY);
	disp_color_str(info, text_color);
	sprintf(info, "NO_PROC: 0x%x.\n", NO_PROC);
	disp_color_str(info, text_color);
	disp_color_str("\n", text_color);
	sprintf(info, "ldt_sel: 0x%x.  ", p->ldt_sel);
	disp_color_str(info, text_color);
	sprintf(info, "ticks: 0x%x.  ", p->ticks);
	disp_color_str(info, text_color);
	sprintf(info, "priority: 0x%x.  ", p->priority);
	disp_color_str(info, text_color);
	sprintf(info, "pid: 0x%x.  ", p->pid);
	disp_color_str(info, text_color);
	sprintf(info, "name: %s.  ", p->name);
	disp_color_str(info, text_color);
	disp_color_str("\n", text_color);
	sprintf(info, "p_flags: 0x%x.  ", p->p_flags);
	disp_color_str(info, text_color);
	sprintf(info, "p_recvfrom: 0x%x.  ", p->p_recvfrom);
	disp_color_str(info, text_color);
	sprintf(info, "p_sendto: 0x%x.  ", p->p_sendto);
	disp_color_str(info, text_color);
	sprintf(info, "nr_tty: 0x%x.  ", p->nr_tty);
	disp_color_str(info, text_color);
	disp_color_str("\n", text_color);
	sprintf(info, "has_int_msg: 0x%x.  ", p->has_int_msg);
	disp_color_str(info, text_color);
}

PUBLIC void dump_msg(const char * title, struct message* msg)
{
	int packed = 0;
	printf("{%s}<0x%x>{%ssrc:%s(%d),%stype:%d,%s(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)%s}%s",
		   title,
		   (int)msg,
		   packed ? "" : "\n        ",
		   proc_table[msg->source].name,
		   msg->source,
		   packed ? " " : "\n        ",
		   msg->type,
		   packed ? " " : "\n        ",
		   msg->u.m3.m3i1,
		   msg->u.m3.m3i2,
		   msg->u.m3.m3i3,
		   msg->u.m3.m3i4,
		   (int)msg->u.m3.m3p1,
		   (int)msg->u.m3.m3p2,
		   packed ? "" : "\n",
		   packed ? "" : "\n"
		);
}

PUBLIC void inform_int(int proc_nr)
{
	struct process* p = proc_table + proc_nr;
	if (p->p_flags & RECEIVING && (p->p_recvfrom == INTERRUPT || p->p_recvfrom == ANY))
	{
		p->p_msg->source = INTERRUPT;
		p->p_msg->type = HARD_INT;
		p->p_msg = 0;
		p->has_int_msg = 0;
		p->p_flags &= ~RECEIVING;
		p->p_recvfrom = NO_PROC;
		assert(p->p_flags == 0);
		unblock(p);
		assert(p->p_flags == 0);
		assert(p->p_msg == 0);
		assert(p->p_recvfrom == NO_PROC);
		assert(p->p_sendto == NO_PROC);
	}
	else
	{
		p->has_int_msg = 1;
	}
}

/**********/

PRIVATE void block(struct process* p_proc)
{
	assert(p_proc->p_flags);
	schedule();
}

PRIVATE void unblock(struct process* p_proc)
{
	assert(p_proc->p_flags == 0);
}

PRIVATE int msg_send(struct process* cnt, int dst, struct message* msg)
{
	struct process* sender = cnt;
	struct process* p_dst = proc_table + dst;
	assert(proc2pid(sender) != dst);
	if (deadlock(proc2pid(sender), dst))
	{
		panic(">>DEADLOCK<< %s->%s", sender->name, p_dst->name);
	}
	if ((p_dst->p_flags & RECEIVING) && (p_dst->p_recvfrom == proc2pid(sender) || p_dst->p_recvfrom == ANY))
	{
		assert(p_dst->p_msg);
		assert(msg);
		memcpy(va2la(dst, p_dst->p_msg), va2la(proc2pid(sender), msg), sizeof(struct message));
		p_dst->p_msg = 0;
		p_dst->p_flags &= ~RECEIVING; /* dest has received the msg */
		p_dst->p_recvfrom = NO_PROC;
		unblock(p_dst);
		assert(p_dst->p_flags == 0);
		assert(p_dst->p_msg == 0);
		assert(p_dst->p_recvfrom == NO_PROC);
		assert(p_dst->p_sendto == NO_PROC);
		assert(sender->p_flags == 0);
		assert(sender->p_msg == 0);
		assert(sender->p_recvfrom == NO_PROC);
		assert(sender->p_sendto == NO_PROC);
	}
	else
	{
		sender->p_flags |= SENDING;
		assert(sender->p_flags == SENDING);
		sender->p_sendto = dst;
		sender->p_msg = msg;
		struct process* p;
		if (p_dst->q_sending)
		{
			p = p_dst->q_sending;
			while (p->next_sending)
			{
				p = p->next_sending;
			}
			p->next_sending = sender;
		}
		else
		{
			p_dst->q_sending = sender;
		}
		sender->next_sending = 0;
		block(sender);
		assert(sender->p_flags == SENDING);
		assert(sender->p_msg != 0);
		assert(sender->p_recvfrom == NO_PROC);
		assert(sender->p_sendto == dst);
	}
	return 0;
}

PRIVATE int msg_receive(struct process* cnt, int src, struct message* _msg)
{
	struct process* p_who_wanna_recv = cnt;
	struct process* p_from = 0;
	struct process* prev = 0;
	int copyok = 0;
	assert(proc2pid(p_who_wanna_recv) != src);
	if (p_who_wanna_recv->has_int_msg && (src == ANY || src == INTERRUPT))
	{
		struct message msg;
		reset_msg(&msg);
		msg.source = INTERRUPT;
		msg.type = HARD_INT;
		assert(_msg);
		memcpy(va2la(proc2pid(p_who_wanna_recv), _msg), &msg, sizeof(struct message));
		p_who_wanna_recv->has_int_msg = 0;
		assert(p_who_wanna_recv->p_flags == 0);
		assert(p_who_wanna_recv->p_msg == 0);
		assert(p_who_wanna_recv->p_sendto == NO_PROC);
		assert(p_who_wanna_recv->has_int_msg == 0);
		return 0;
	}
	if (src == ANY)
	{
		if (p_who_wanna_recv->q_sending)
		{
			p_from = p_who_wanna_recv->q_sending;
			copyok = 1;
			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_PROC);
			assert(p_who_wanna_recv->p_sendto == NO_PROC);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_PROC);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}
	else if (src >= 0 && src < NR_SYSU_PROCS + NR_USER_PROCS)
	{
		p_from = &proc_table[src];

		if ((p_from->p_flags & SENDING) && (p_from->p_sendto == proc2pid(p_who_wanna_recv)))
		{
			copyok = 1;
			struct process* p = p_who_wanna_recv->q_sending;
			assert(p);
			while (p)
			{
				assert(p_from->p_flags & SENDING);
				if (proc2pid(p) == src)
				{
					break;
				}
				prev = p;
				p = p->next_sending;
			}
			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_PROC);
			assert(p_who_wanna_recv->p_sendto == NO_PROC);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_PROC);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}
	if (copyok)
	{
		if (p_from == p_who_wanna_recv->q_sending)
		{
			assert(prev == 0);
			p_who_wanna_recv->q_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}
		else
		{
			assert(prev);
			prev->next_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}
		assert(_msg);
		assert(p_from->p_msg);
		memcpy(va2la(proc2pid(p_who_wanna_recv), _msg), va2la(proc2pid(p_from), p_from->p_msg), sizeof(struct message));
		p_from->p_msg = 0;
		p_from->p_sendto = NO_PROC;
		p_from->p_flags &= ~SENDING;
		unblock(p_from);
	}
	else
	{
		p_who_wanna_recv->p_flags |= RECEIVING;
		p_who_wanna_recv->p_msg = _msg;
		p_who_wanna_recv->p_recvfrom = src;
		block(p_who_wanna_recv);
		assert(p_who_wanna_recv->p_flags == RECEIVING);
		assert(p_who_wanna_recv->p_msg != 0);
		assert(p_who_wanna_recv->p_recvfrom != NO_PROC);
		assert(p_who_wanna_recv->p_sendto == NO_PROC);
		assert(p_who_wanna_recv->has_int_msg == 0);
	}
	return 0;
}

PRIVATE int deadlock(int src, int dst)
{
	struct process* p = proc_table + dst;
	while (1)
	{
		if (p->p_flags & SENDING)
		{
			if (p->p_sendto == src)
			{
				p = proc_table + dst;
#ifdef CL_DEBUG
				printf("=_=%s", p->name);
#endif
				do
				{
					assert(p->p_msg);
					p = proc_table + p->p_sendto;
#ifdef CL_DEBUG
					printf("->%s", p->name);
#endif
				} while (p != proc_table + src);
#ifdef CL_DEBUG
				printf("=_=");
#endif
				return 1;
			}
			p = proc_table + p->p_sendto;
		}
		else
		{
			break;
		}
	}
	return 0;
}