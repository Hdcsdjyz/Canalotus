/***
 * @file sys.c
 * @author Lhxl
 * @date 2024-12-28
 * @version build30
 * @brief 微内核消息机制
 ***/

#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/global.h"

PUBLIC void sys_sys()
{
	struct message msg;
	while (1) {
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;

		switch (msg.type) {
		case GET_TICKS:
			msg.RETVAL = ticks;
			send_recv(SEND, src, &msg);
			break;
		default:
			panic("unknown msg type");
			break;
		}
	}
}