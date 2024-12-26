/***
 * @file global.c
 * @author Lhxl
 * @date 2024-12-21
 * @version 1.0
 * @brief 硬盘
 ***/

#ifndef _CANALOTUS_HD_H_
#define _CANALOTUS_HD_H_

#include "type.h"
#include "const.h"

/***
 * @struct part_ent
 * @brief 硬盘分区
 ***/
struct part_ent
{
	u8 boot_ind;			///< 指示器
	u8 start_head;			///< 开始磁头
	u8 start_sector;		///< 0~5：开始扇区 6~7：开始柱道8~9
	u8 start_cylinder;		///< 开始柱道0~7
	u8 sys_id;				///< 系统id(01:FAT12, 83:Linux, )
	u8 end_head;			///< 结束磁头
	u8 end_sector;			///< 结束扇区，同上
	u8 end_cylinder;		///< 结束柱道，同上
	u32 start_sect;			///<
	u32 nr_sect;			///< 此分区的扇区数
};

#define REG_DATA		0X1F0
#define REG_FEATURES	0X1F1
#define REG_ERROR		REG_FEATURES
#define REG_NSECTOR		0x1F2
#define REG_LBA_LOW		0x1F3
#define REG_LBA_MID		0x1F4
#define REG_LBA_HIGH	0x1F5
#define REG_DEVICE		0x1F6
#define REG_STATUS		0x1F7

#define STATUS_BSY		0x80
#define STATUS_DRDY		0x40
#define STATUS_DFSE		0x20
#define STATUS_DSC		0x10
#define STATUS_DRQ		0x08
#define STATUS_CORR		0x04
#define STATUS_IDX		0x02
#define STATUS_ERR		0x01

#define REG_CMD			REG_STATUS
#define REG_DEV_CTRL	0x3F6
#define REG_ALT_STATUS	REG_DEV_CTRL
#define REG_DRV_ADDR	0x3F7

struct hd_cmd
{
	u8 features;
	u8 count;
	u8 lba_low;
	u8 lba_mid;
	u8 lba_high;
	u8 device;
	u8 command;
};

/***
 * @struct part_info
 * @brief 分区信息
 ***/
struct part_info
{
	u32 base;
	u32 size;
};

/***
 * @struct hd_info
 * @brief 磁盘分区信息
 ***/
struct hd_info
{
	int open_cnt;
	struct part_info primary[NR_PRIM_PER_DRIVE];
	struct part_info logical[NR_SUB_PER_DRIVE];
};

#define HD_TIMEOUT					10000
#define PARTITION_TABLE_OFFSET		0x1BE
#define ATA_IDENTIFY				0xEC
#define ATA_READ					0x20
#define ATA_WRITE					0x30

#define MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) | ((drv) << 4) | (lba_highest & 0xF) | 0xA0)

#endif