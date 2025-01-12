/***
 * @file fs.h
 * @author Lhxl
 * @date 2025-1-13
 * @version build30
 * @brief 文件系统
 ***/

#ifndef _CANALOTUS_FS_H_
#define _CANALOTUS_FS_H_

#include "type.h"

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
#define FS_V_0_0_1	0x1
#define FILESYSTEM_VERSION		FS_V_0_0_1

/***
 * @struct Canalotus_Filesystem
 * @brief 文件系统信息
 * @note "~"标记为仅存在内存中的数据
 ***/
struct Canalotus_Filesystem
{
	u32 version;				///< 版本号
	u32 nr_sectors;				///< 扇区数
	u32 nr_sector_map;			///< sector_map数
	u32 file_desc_size;			///< 文件描述符大小
	u32 nr_file_desc;			///< 文件数
	u32 sp_file_desc_sector;	///< 特殊文件描述符扇区号
	u32 start_sector;			///< 文件起始扇区号
	u32 unused1;				///< 未使用
	u32 unused2;				///< 未使用
	u32 unused3;				///< 未使用
	u32 unused4;				///< 未使用
	u32 unused5;				///< 未使用
	u32 unused6;				///< 未使用
	u32 unused7;				///< 未使用
	u32 unused8;				///< 未使用
	u32 unused9;				///< 未使用

	int sb_dev;						///< ~存储超级块的设备
};

/***
 * @def SUPER_BLOCK_SIZE
 * @brief 存储在硬盘中的超级块大小
 ***/
#define FILESYSTEM_SIZE (sizeof(struct Canalotus_Filesystem) - sizeof(int))

/***
 * @struct file_desc
 * @brief 文件描述符
 * @note "~"标记为仅存在内存中的数据
 ***/
struct file_desc
{
	char name[32];		///< 文件名
	u32 type;			///< 文件类型
	u32 size;			///< 文件大小
	u32 start_sector;		///< 文件的第一个扇区号
	u32 nr_sectors;		///< 文件占用的扇区数
	//u32 _unused;		///< 未使用

	int dev;			///< ~设备号
	int cnt;			///< ~占用此文件的进程数
	int num;			///< ~file_desc号
};

/***
 * @def FILE_DESC_SIZE
 * @brief 存储在硬盘中的file_desc大小
 ***/
#define FILE_DESC_SIZE (sizeof(struct file_desc) - sizeof(int) * 3)

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
	int file_desc_nr;				///< file_desc号
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

/***
 * @struct file_desc
 * @brief 文件描述符
 ***/
struct file_occupy_desc
{
	int fs_mode;				///< 读写模式
	int fs_pos;					///< 当前操作位置
	struct file_desc* fd;		///< inode指针
};






#endif