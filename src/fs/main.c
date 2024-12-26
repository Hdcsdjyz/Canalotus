/***
 * @file main.c
 * @author Lhxl
 * @date 2024-12-21
 * @version build26
 * @brief 文件系统
 ***/

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