/***
 * @file main.c
 * @author Lhxl
 * @date 2025-1-13
 * @version build30
 * @brief 文件系统
 ***/

#include "../include/sys/config.h"
#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/global.h"
#include "../include/sys/hd.h"
#include "../include/string.h"
#include "../include/assert.h"
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
	struct Canalotus_Filesystem filesystem;
	int bits_per_sect = SECTOR_SIZE * 8;
	filesystem.version = FILESYSTEM_VERSION;
	filesystem.nr_sectors = geo.size;
	filesystem.nr_sector_map = filesystem.nr_sectors / bits_per_sect + 1;
	filesystem.file_desc_size = FILE_DESC_SIZE;
	filesystem.nr_file_desc = 1;
	filesystem.sp_file_desc_sector = 1 + 1 + filesystem.nr_sector_map;
	filesystem.start_sector = 1 + 1 + 1 + filesystem.nr_sector_map;
	memset(fsbuf, 0, SECTOR_SIZE);
	memcpy(fsbuf, &filesystem, FILESYSTEM_SIZE);
	WRITE_SECT(ROOT_DEV, 1);
	/* sector_map */
	memset(fsbuf, 0, SECTOR_SIZE);
	int nr_sectors = filesystem.nr_sector_map + 2;
	int i = 0;
	for (i = 0; i < nr_sectors / 8; i++)
	{
		fsbuf[i] = 0xFF;
	}
	for (int j = 0;j < nr_sectors % 8; j++)
	{
		fsbuf[i] |= 1 << j;
	}
	WRITE_SECT(ROOT_DEV, 2);
	/* main_file_desc
	 * 此文件存储所有文件描述符，相当于主文件夹 */
	memset(fsbuf, 0, SECTOR_SIZE);
	struct file_desc* main_file_desc = (struct file_desc*) fsbuf;
	main_file_desc->type = SPECIAL_FILE;
	main_file_desc->size = 0;
	main_file_desc->start_sector = filesystem.start_sector;
	main_file_desc->nr_sectors = 1;
	WRITE_SECT(ROOT_DEV, filesystem.file_desc_sector);
	/* main_file */
	memset(fsbuf, 0, SECTOR_SIZE);
	WRITE_SECT(ROOT_DEV, filesystem.start_sector);
}