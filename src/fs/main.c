#include "../include/const.h"
#include "../include/proto.h"

PUBLIC void sys_fs()
{
	printf("sys_fs begins.\n");
	struct message driver_msg;
	driver_msg.type = DEV_OPEN;
	send_recv(BOTH, SYSPROC_HD, &driver_msg);
	spin("FS");
}