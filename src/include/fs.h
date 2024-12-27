/***
 * @file fs.h
 * @author Lhxl
 * @date 2024-12-27
 * @version build29
 * @brief 文件系统
 ***/

#ifndef _CANALOTUS_FS_H_
#define _CANALOTUS_FS_H_

#include "type.h"
#include "global.h"

/***
 * @struct dev_drv_map
 * @brief 驱动程序表
 ***/
struct dev_drv_map
{
    int driver_nr;	///< 驱动号
};

/***
 * @def FILESYSTEM_VERSION
 * @brief 文件系统版本
 * @version 1.0
 ***/
#define FILESYSTEM_VERSION_1	0x1
#define FILESYSTEM_VERSION		FILESYSTEM_VERSION_1

/***
 * @struct super_block
 * @brief 超级块
 * @note "~"标记为仅存在内存中的数据
 ***/
struct super_block
{
	u32 magic;					///< 魔数
	u32 nr_inodes;				///< 总inode数量
	u32 nr_sects;				///< 总扇区数量
	u32 nr_imap_sects;			///< inode_map占用扇区数
	u32 nr_smap_sects;			///< sector_map占用扇区数
	u32 n_1st_sect;				///< 第一个数据扇区号
	u32 nr_inode_sects;			///< inode扇区数
	u32 root_inode;				///< 根目录的inode号
	u32 inode_size;				///< inode大小
	u32 inode_isize_offset;		///< struct inode::i_size的偏移
	u32 inode_start_offset;		///< struct inode::i_start_sect的偏移
	u32 dir_ent_size;			///< dir_entry的大小
	u32 dir_ent_inode_offset;	///< struct dir_entry::inode_nr的偏移
	u32 dir_ent_fname_offset;	///< struct dir_entry::name的偏移

	int sb_dev;					///< ~存储超级块的设备
};

/***
 * @def SUPER_BLOCK_SIZE
 * @brief 存储在硬盘中的超级块大小
 ***/
#define SUPER_BLOCK_SIZE (sizeof(struct super_block) - sizeof(int))

/***
 * @struct inode
 * @brief i_node
 * @note "~"标记为仅存在内存中的数据
 ***/
struct inode
{
	u32 i_type;			///< 文件类型
	u32 i_size;			///< 文件大小
	u32 i_start_sect;	///< 文件的第一个扇区号
	u32 i_nr_sects;		///< 文件占用的扇区数
	u8 _unused[16];		///< 用以数据对齐

	int i_dev;			///< ~设备号
	int i_cnt;			///< ~占用此文件的进程数
	int i_num;			///< ~inode号
};

/***
 * @def INODE_SIZE
 * @brief 存储在硬盘中的inode大小
 ***/
#define INODE_SIZE (sizeof(struct inode) - sizeof(int) * 3)

/***
 * @def MAX_FILENAME_SIZE
 * @brief 文件名大小最大值
 **/
#define MAX_FILENAME_SIZE 12

/***
 * @struct dir_entry
 * @brief 文件入口
 ***/
struct dir_entry
{
	int inode_nr;					///< inode号
	char name[MAX_FILENAME_SIZE];	///< 文件名称
};

/***
 * @def DIR_ENTRY_SIZE
 * @brief 文件入口大小
 **/
#define DIR_ENTRY_SIZE sizeof(struct dir_entry)

/***
 * @def READ_SECT
 * @param dev		设备号
 * @param sect_nr	扇区号
 * @brief 读扇区
 **/
#define READ_SECT(dev, sect_nr) rw_sector(DEV_READ, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, SYSPROC_FS, fsbuf)
/***
 * @def WRITE_SECT
 * @param dev		设备号
 * @param sect_nr	扇区号
 * @brief 写扇区
 **/
#define WRITE_SECT(dev, sect_nr) rw_sector(DEV_WRITE, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, SYSPROC_FS, fsbuf)








#endif