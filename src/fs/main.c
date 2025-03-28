/***
 * @file main.c
 * @author Lhxl
 * @date 2025-3-27
 * @version build31
 * @brief 文件系统
 ***/

#include "../include/sys/config.h"
#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/global.h"
#include "../include/sys/hd.h"
#include "../include/sys/type.h"
#include "../include/string.h"
#include "../include/assert.h"
#include "../include/stdio.h"
#include "../include/sys/fs.h"

PRIVATE void init_fs();
PRIVATE void mkfs();

/***
 * @note <Ring1, PUBLIC>
 ***/
PUBLIC void sys_fs()
{
	init_fs();
	spin("FS");
}

/***
 * @param io_type	读/写
 * @param dev		设备号
 * @param pos		操作位置的偏移
 * @param bytes		要操作的字节数
 * @param proc_nr	缓存来源的进程
 * @param buf		读写缓存
 * @return 如果成功返回 0
 * @note <Ring1, PUBLIC>
 ***/
PUBLIC int rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr, void* buf)
{
	struct message driver_msg;
	driver_msg.type = io_type;
	driver_msg.DEVICE = MINOR(dev);
	driver_msg.POSITION = pos;
	driver_msg.BUF = buf;
	driver_msg.CNT = bytes;
	driver_msg.PROC_NR = proc_nr;
	assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &driver_msg);
	return 0;
}

/***
 * @note <Ring1, PRIVATE>
 ***/
PRIVATE void init_fs()
{
	struct message driver_msg;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.type = DEV_OPEN;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	mkfs();
}

/***
 * @note <Ring1, PRIVATE>
 ***/
PRIVATE void mkfs()
{
	struct message driver_msg;
	struct part_info geo;
	driver_msg.type = DEV_IOCTL;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.REQUEST = DIOCTL_GET_GEO;
	driver_msg.BUF = &geo;
	driver_msg.PROC_NR = SYSPROC_FS;
	assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
#ifdef CL_DEBUG
	printf("dev size: 0x%x sectors\n", geo.size);
#endif

	/* filesystem */
	memset(fsbuf, 0, CLUSTER_SIZE);
	struct Filesystem fs;
	fs.version = FILESYSTEM_VERSION;
	fs.nr_sectors = geo.size;
	fs.nr_clusters = geo.size / 8;
	fs.nr_cluster_map = fs.nr_clusters / 8 / 512 / 8;
	fs.nr_file_desc = 1;
	fs.file_table_cluster[0] = 1 + 1 + fs.nr_cluster_map;
	fs.start_cluster = 1 + 1 + fs.nr_cluster_map + 1;
	fs.dev = 0;
	memcpy(fsbuf, &fs, FILESYSTEM_SIZE);
	WRITE_SECT(ROOT_DEV, 1);

	/* cluster_map */
	memset(fsbuf, 0, CLUSTER_SIZE);
	for (int i = 2; i < fs.nr_cluster_map; i++)
	{
		WRITE_SECT(ROOT_DEV, i);
	}
	struct Cluster_map cmap;
	memset(&cmap, 0, CLUSTER_SIZE);
	for (int i = 0; i < 1 + 1 + fs.nr_cluster_map + 1; i++)
	{
		cmap.cluster[i] = TRUE;
	}
	memcpy(fsbuf, &cmap, CLUSTER_SIZE);
	WRITE_SECT(ROOT_DEV, 2);

	/* FileTable */
	memset(fsbuf, 0, CLUSTER_SIZE);
	WRITE_SECT(ROOT_DEV, fs.file_table_cluster[0]);
}