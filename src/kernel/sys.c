/***
 * @file sys.c
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief 微内核消息机制
 ***/

#include "../include/const.h"
#include "../include/proto.h"
#include "../include/global.h"

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