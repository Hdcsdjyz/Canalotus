/***
 * @file fs.h
 * @author Lhxl
 * @date 2025-3-27
 * @version build31
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
 ***/
#define FS_V_0_0_1	0x1
#define FILESYSTEM_VERSION		FS_V_0_0_1

/***
 * @struct Filesystem
 * @brief 文件系统信息
 * @note "~"标记为仅存在内存中的数据\n
 * 文件表是一片连续的空间，存储文件描述符
 ***/
struct Filesystem
{
	u32 version;			///< 版本号
	u32 nr_sectors;			///< 扇区数
	u32 nr_clusters;		///< 簇数
	u32 nr_cluster_map;		///< cluster_map数
	u32 file_desc_size;		///< 文件描述符大小
	u32 nr_file_desc;		///< 文件数
	u32 file_table_cluster[10]; ///< 文件表簇号
	u32 start_cluster;		///< 文件起始簇号
	u32 dev;				///< ~存储超级块的设备
	u32 unused[1006];		///< 未使用

};

/***
 * @def SUPER_BLOCK_SIZE
 * @brief 存储在硬盘中的超级块大小
 ***/
#define FILESYSTEM_SIZE (sizeof(struct Filesystem) - sizeof(int))

/***
 * @struct file_desc
 * @brief 文件描述符
 * @note 占用簇数小于9时直接存储，否则单独存储
 ***/
struct File_desc
{
	u32 type;			///< 文件类型
	u32 size;			///< 文件大小
	u32 cluster[10];	///< 文件所在簇号
	u32 nr_clusters;	///< 文件占用的簇数
	u32 num;			///< file_desc号
	u32 dev;			///< 设备号
	u32 cnt;			///< 占用此文件的进程数
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
#define MAX_FILENAME_SIZE 32

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
#define READ_SECT(dev, sect_nr) rw_sector(DEV_READ, dev, (sect_nr) * CLUSTER_SIZE, CLUSTER_SIZE, SYSPROC_FS, fsbuf)
/***
 * @def WRITE_SECT
 * @param dev		设备号
 * @param sect_nr	扇区号
 * @brief 写扇区
 **/
#define WRITE_SECT(dev, sect_nr) rw_sector(DEV_WRITE, dev, (sect_nr) * CLUSTER_SIZE, CLUSTER_SIZE, SYSPROC_FS, fsbuf)

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

struct Cluster_map
{
	_Bool cluster[CLUSTER_SIZE * 8];
};

struct FileTable
{
	struct File_desc fd[64];
};

struct FileCluster
{
	u32 cluster[1023];
	u32 next_cluster;
};

#endif