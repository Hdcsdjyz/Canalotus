/***
 * @file main.c
 * @author Lhxl
 * @date 2024-12-26
 * @version build27
 * @brief 文件系统
 ***/

#include "../include/const.h"
#include "../include/proto.h"
#include "../include/global.h"

PUBLIC void sys_fs()
{
	printf("sys_fs begins.\n");
	struct message driver_msg;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.type = DEV_OPEN;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	spin("FS");
}