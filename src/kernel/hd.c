/***
 * @file hd.c
 * @author Lhxl
 * @date 2024-12-28
 * @version build39
 * @brief 硬盘驱动
 ***/

#include "../include/sys/const.h"
#include "../include/sys/proto.h"
#include "../include/sys/hd.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/assert.h"

PRIVATE void init_hd();
PRIVATE void hd_identify(int drive);
PRIVATE void hd_cmd_out(struct hd_cmd *cmd);
PRIVATE void interrupt_wait();
PRIVATE int waitfor(int mask, int val, int timeout);
PRIVATE void hd_open(int device);
PRIVATE void hd_close(int device);
PRIVATE void hd_rdwt(struct message* msg);
PRIVATE void hd_ioctl(struct message* msg);
PRIVATE void partition(int device, int style);
PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent* entry);
#ifdef CL_DEBUG
PRIVATE void print_identify_info(u16* hd_info);
PRIVATE void print_hdinfo(struct hd_info* hdi);
#endif

PRIVATE u8 hd_status;
PRIVATE u8 hd_buf[SECTOR_SIZE * 2];
PRIVATE struct hd_info hd_info[1];

#define DRV_OF_DEV(dev) (dev <= MAX_PRIM ? dev / NR_PRIM_PER_DRIVE : (dev - MINOR_hd1a) / NR_SUB_PER_DRIVE)

PUBLIC void sys_hd()
{
	struct message msg;
	init_hd();
	while (1)
	{
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		switch (msg.type)
		{
		case DEV_OPEN:
			hd_open(msg.DEVICE);
			break;
		case DEV_CLOSE:
			hd_close(msg.DEVICE);
			break;
		case DEV_READ:
		case DEV_WRITE:
			hd_rdwt(&msg);
			break;
		case DEV_IOCTL:
			hd_ioctl(&msg);
			break;
		default:
			dump_msg("HD driver::unknown msg", &msg);
			spin("FS::main_loop (invalid msg.type)");
			break;
		}
		send_recv(SEND, src, &msg);
	}
}

PUBLIC void hd_handler(int irq)
{
	hd_status = in_byte(REG_STATUS);
	inform_int(SYSPROC_HD);
}

PRIVATE void init_hd()
{
	u8* pNrDrives = (u8*)0x475;
#ifdef CL_DEBUG
	printf("NrDrives:%d.\n", *pNrDrives);
#endif
	assert(*pNrDrives);
	for (int i = 0; i < sizeof(hd_info) / sizeof(hd_info[0]); i++)
	{
		memset(&hd_info[i], 0, sizeof(hd_info[0]));
	}
	hd_info->open_cnt = 0;
	put_irq_handler(AT_WINI_IRQ, hd_handler);
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);

}

PRIVATE void hd_identify(int drive)
{
	struct hd_cmd cmd;
	cmd.device = MAKE_DEVICE_REG(0, drive, 0);
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA, hd_buf, SECTOR_SIZE);
#ifdef CL_DEBUG
	print_identify_info((u16*)hd_buf);
#endif
	u16* hdinfo = (u16*)hd_buf;
	hd_info[drive].primary[0].base = 0;
	hd_info[drive].primary[0].size = ((int)hdinfo[61] << 16) + hdinfo[60];
}

PRIVATE void hd_cmd_out(struct hd_cmd *cmd)
{
	if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
	{
		panic("hd error");
	}
	out_byte(REG_DEV_CTRL, 0);
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR, cmd->count);
	out_byte(REG_LBA_LOW, cmd->lba_low);
	out_byte(REG_LBA_MID, cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE, cmd->device);
	out_byte(REG_CMD, cmd->command);
}

PRIVATE void interrupt_wait()
{
	struct message msg;
	send_recv(RECEIVE, INTERRUPT, &msg);
}

#ifdef CL_DEBUG
PRIVATE void print_identify_info(u16* hd_info)
{
	int j = 0;
	char str[64];
	struct iden_info_ascii
	{
		int idx;
		int len;
		char* desc;
	} iinfo[] = {{10, 20, "HD SN"}, {27, 40, "HD Model"}};
	for (int i = 0; i < sizeof(iinfo) / sizeof(iinfo[0]); i++)
	{
		char* p = (char*)&hd_info[iinfo[i].idx];
		for (j = 0; j < iinfo[i].len / 2; j++)
		{
			str[j * 2 + 1] = *p++;
			str[j * 2] = *p++;
		}
		str[j * 2] = 0;
		printf("%s: %s\n", iinfo[i].desc, str);
	}
	int capabilities = hd_info[49];
	printf("LBA_supported: %s\n",
		capabilities & 0x0200 ? "yes" : "no");
	int cmd_set_supported = hd_info[83];
	printf("LBA48_supported: %s\n",
		cmd_set_supported & 0x0400 ? "yes" : "no");
	int sectors = ((int)hd_info[61] << 16) + hd_info[60];
	printf("HD_size: %dMB\n", sectors * 512 / 1000000);
}
#endif

PRIVATE int waitfor(int mask, int val, int timeout)
{
	int t = get_ticks();
	while ((get_ticks() - t) * 1000 / HZ < timeout)
	{
		if ((in_byte(REG_STATUS) & mask) == val)
		{
			return 1;
		}
	}
	return 0;
}

PRIVATE void hd_open(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0);
	hd_identify(drive);
	if (hd_info[drive].open_cnt++ == 0)
	{
		partition(drive * (NR_PART_PER_DRIVE + 1), P_PRIMARY);
		print_hdinfo(&hd_info[drive]);
	}
}

PRIVATE void partition(int device, int style)
{
	int drive = DRV_OF_DEV(device);
	struct hd_info* hdi = &hd_info[drive];
	struct part_ent part_tbl[NR_SUB_PER_DRIVE];
	if (style == P_PRIMARY)
	{
		get_part_table(drive, drive, part_tbl);
		int nr_prim_parts = 0;
		for (int i = 0; i < NR_PART_PER_DRIVE; i++)
		{
			if (part_tbl[i].sys_id == NO_PART)
			{
				continue;
			}
			nr_prim_parts++;
			int dev_nr = i + 1;
			hdi->primary[dev_nr].base = part_tbl[i].start_sect;
			hdi->primary[dev_nr].size = part_tbl[i].nr_sects;
			if (part_tbl[i].sys_id == EXT_PART)
			{
				partition(device + dev_nr, P_EXTENDED);
			}
		}
		assert(nr_prim_parts != 0);
	}
	else if (style == P_EXTENDED)
	{
		int j = device % NR_PRIM_PER_DRIVE;
		int ext_start_sect = hdi->primary[j].base;
		int s = ext_start_sect;
		int nr_1st_sub = (j - 1) * NR_SUB_PER_PART;
		for (int i = 0; i < NR_SUB_PER_PART; i++)
		{
			int dev_nr = nr_1st_sub + i;
			get_part_table(drive, s, part_tbl);
			hdi->logical[dev_nr].base = s + part_tbl[0].start_sect;
			hdi->logical[dev_nr].size = part_tbl[0].nr_sects;
			s = ext_start_sect + part_tbl[1].start_sect;
			if (part_tbl[1].sys_id == NO_PART)
			{
				break;
			}
		}
	}
	else
	{
		assert(0);
	}
}

PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent* entry)
{
	struct hd_cmd cmd;
	cmd.features = 0;
	cmd.count = 1;
	cmd.lba_low = sect_nr & 0xFF;
	cmd.lba_mid = sect_nr >> 8 & 0xFF;
	cmd.lba_high = sect_nr >> 16 & 0xFF;
	cmd.device = MAKE_DEVICE_REG(1, drive, sect_nr >> 24 & 0xF);
	cmd.command = ATA_READ;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA, hd_buf, SECTOR_SIZE);
	memcpy(entry, hd_buf + PARTITION_TABLE_OFFSET, sizeof(struct part_ent) * NR_PART_PER_DRIVE);
}

#ifdef CL_DEBUG
PRIVATE void print_hdinfo(struct hd_info* hdi)
{
	for (int i = 0;i < NR_PART_PER_DRIVE + 1; i++)
	{
		printf("%sPART_%d: base %d(0x%x), size %d(0x%x)) (in sector)\n",
			i == 0 ? " " : "     ",
			i,
			hdi->primary[i].base,
			hdi->primary[i].base,
			hdi->primary[i].size,
			hdi->primary[i].size);
	}
	for (int i = 0; i < NR_SUB_PER_DRIVE; i++)
	{
		if (hdi->logical[i].size == 0)
		{
			continue;
		}
		printf("         "
			"%d: base %d(0x%x), size %d(0x%x)) (in sector)\n",
			i,
			hdi->logical[i].base,
			hdi->logical[i].base,
			hdi->logical[i].size,
			hdi->logical[i].size);
	}
}
#endif

PRIVATE void hd_close(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0);
	hd_info[drive].open_cnt--;
}

PRIVATE void hd_rdwt(struct message* msg)
{
	int drive = DRV_OF_DEV(msg->DEVICE);
	u64 pos = msg->POSITION;
	assert((pos >> SECTOR_SIZE_SHIFT) < 1 << 31);
	assert((pos & 0x1FF) == 0);
	u32 sect_nr = pos >> SECTOR_SIZE_SHIFT;
	int logidx = (msg->DEVICE - MINOR_hd1a) % NR_SUB_PER_DRIVE;
	sect_nr += msg->DEVICE < MAX_PRIM ? hd_info[drive].primary[msg->DEVICE].base : hd_info[drive].primary[logidx].base;
	struct hd_cmd cmd;
	cmd.features = 0;
	cmd.count = (msg->CNT + SECTOR_SIZE - 1) / SECTOR_SIZE;
	cmd.lba_low = sect_nr & 0xFF;
	cmd.lba_mid = sect_nr >> 8 & 0xFF;
	cmd.lba_high = sect_nr >> 16 & 0xFF;
	cmd.device = MAKE_DEVICE_REG(1, drive, sect_nr >> 24 & 0xF);
	cmd.command = msg->type == DEV_READ ? ATA_READ : ATA_WRITE;
	hd_cmd_out(&cmd);
	int bytes_left = msg->CNT;
	void* la = va2la(msg->PROC_NR, msg->BUF);
	while (bytes_left)
	{
		int bytes = MIN(SECTOR_SIZE, bytes_left);
		if (msg->type == DEV_READ)
		{
			interrupt_wait();
			port_read(REG_DATA, hd_buf, SECTOR_SIZE);
			memcpy(la, va2la(SYSPROC_HD, hd_buf), bytes);
		}
		else
		{
			if (!waitfor(STATUS_DRQ, STATUS_DRQ, HD_TIMEOUT))
			{
				panic("hd writing error.");
				interrupt_wait();
			}
		}
		bytes_left -= SECTOR_SIZE;
		la += SECTOR_SIZE;
	}
}

PRIVATE void hd_ioctl(struct message* msg)
{
	int device = msg->DEVICE;
	int drive = DRV_OF_DEV(device);
	struct hd_info* hdi = &hd_info[drive];
	if (msg->REQUEST == DIOCTL_GET_GEO)
	{
		void* dst = va2la(msg->PROC_NR, msg->BUF);
		void* src = va2la(SYSPROC_HD, device < MAX_PRIM ? &hdi->primary[device] : &hdi->logical[(device - MINOR_hd1a) % NR_SUB_PER_DRIVE]);
		memcpy(dst, src, sizeof(struct part_info));
	}
	else
	{
		assert(0);
	}
}