/***
 * @file proc.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 进程调度
 ***/

#include "../include/const.h"
#include "../include/proto.h"
#include "../include/global.h"
#include "../include/string.h"

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
		for (p = proc_table; p < proc_table + NR_SYSU_PROCS + NR_USER_PROCS; p++)
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
			for (p = proc_table; p < proc_table + NR_SYSU_PROCS + NR_USER_PROCS; p++)
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
	assert((src_dst >= 0 && src_dst < NR_SYSU_PROCS + NR_USER_PROCS) || src_dst ==ANY || src_dst == INTERRUPT);
	int ret = 0;
	int caller = proc2pid(p_proc);
	struct message* mla = (struct message*)va2la(caller, msg);
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
	if ((p_who_wanna_recv->has_int_msg) && ((src == ANY) || (src == INTERRUPT)))
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
				printf("=_=%s", p->name);
				do
				{
					assert(p->p_msg);
					p = proc_table + p->p_sendto;
					printf("->%s", p->name);
				} while (p != proc_table + src);
				printf("=_=");
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