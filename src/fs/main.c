/***
 * @file main.c
 * @author Lhxl
 * @date 2024-12-27
 * @version build29
 * @brief 文件系统
 ***/

#include "../include/config.h"
#include "../include/const.h"
#include "../include/proto.h"
#include "../include/global.h"
#include "../include/hd.h"
#include "../include/string.h"

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
	/* super_block */
	struct super_block sb;
	int bits_per_sect = SECTOR_SIZE * 8;
	sb.magic = FILESYSTEM_VERSION_1;
	sb.nr_inodes = bits_per_sect;
	sb.nr_inode_sects = sb.nr_inodes * INODE_SIZE;
	sb.nr_sects = geo.size;
	sb.nr_imap_sects = 1;
	sb.nr_smap_sects = sb.nr_sects;
	sb.n_1st_sect = 1 + 1 + sb.nr_imap_sects + sb.nr_smap_sects;
	sb.root_inode = ROOT_INODE;
	sb.inode_size = INODE_SIZE;
	struct inode x;
	sb.inode_isize_offset = (int)&x.i_size - (int)&x;
	sb.inode_start_offset = (int)&x.i_start_sect - (int)&x;
	sb.dir_ent_size = DIR_ENTRY_SIZE;
	struct dir_entry de;
	sb.dir_ent_inode_offset = (int)&de.inode_nr - (int)&de;
	sb.dir_ent_fname_offset = (int)&de.name - (int)&de;
	memset(fsbuf, 0x90, SECTOR_SIZE);
	memcpy(fsbuf, &sb, SUPER_BLOCK_SIZE);
	WRITE_SECT(ROOT_DEV, 1);
#ifdef CL_DEBUG
	printf("devbase:0x%x00, sb:0x%x00, imap:0x%x00, smap:0x%x00\n"
		"        inodes:0x%x00, 1st_sector:0x%x00\n",
		geo.base * 2,
		(geo.base + 1) * 2,
		(geo.base + 2) * 2,
		(geo.base + 2 + sb.nr_imap_sects) * 2,
		(geo.base + 2 + sb.nr_imap_sects + sb.nr_smap_sects) * 2,
		(geo.base + sb.n_1st_sect) * 2);
#endif printf
	/* inode_map */
	memset(fsbuf, 0, SECTOR_SIZE);
	for (int i = 0; i < NR_CONSOLES + 2; i++)
	{
		fsbuf[0] |= 1 << i;
	}
	assert(fsbuf[0] == 0x1F);
	WRITE_SECT(ROOT_DEV, 2);
	/* sector_map */
	memset(fsbuf, 0, SECTOR_SIZE);
	int nr_sects = NR_DEFAULT_FILE_SECTS + 1;
	for (int i = 0; i < nr_sects / 8; i++)
	{
		fsbuf[i] = 0xFF;
	}
	for (int j = 0; j < nr_sects % 8; j++)
	{
		fsbuf[nr_sects / 8 - 1] |= 1 << j;
	}
	WRITE_SECT(ROOT_DEV, 2 + sb.nr_imap_sects);
	memset(fsbuf, 0, SECTOR_SIZE);
	for (int i = 1; i < sb.nr_smap_sects; i++)
	{
		WRITE_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + i);
	}
	/* inodes */
	memset(fsbuf, 0, SECTOR_SIZE);
	struct inode* p_inode = (struct inode*)fsbuf;
	p_inode->i_type = I_DIRECTORY;
	p_inode->i_size = DIR_ENTRY_SIZE * 4;
	p_inode->i_start_sect = sb.n_1st_sect;
	p_inode->i_nr_sects = NR_DEFAULT_FILE_SECTS;
	for (int i = 0; i < NR_CONSOLES; i++)
	{
		p_inode = (struct inode*)(fsbuf + INODE_SIZE * (i + 1));
		p_inode->i_type = I_CHAR_SPECIAL;
		p_inode->i_size = 0;
		p_inode->i_start_sect = MAKE_DEV(DEV_CHAR_TTY, i);
		p_inode->i_nr_sects = 0;
	}
	WRITE_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + sb.nr_smap_sects);
	/* "/" */
	memset(fsbuf, 0, SECTOR_SIZE);
	struct dir_entry* p_de = (struct dir_entry*)fsbuf;
	p_de->inode_nr = 1;
	strcpy(p_de->name, "/");
	for (int i = 0; i < NR_CONSOLES; i++)
	{
		p_de++;
		p_de->inode_nr = i + 2;
		sprintf(p_de->name, "dev_tty%d", i);
	}
	WRITE_SECT(ROOT_DEV, sb.n_1st_sect);
}