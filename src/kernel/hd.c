/***
 * @file hd.c
 * @author Lhxl
 * @date 2024-12-25
 * @version 1.0
 * @brief 硬盘驱动
 ***/
#include "../include/const.h"
#include "../include/proto.h"
#include "../include/hd.h"

PRIVATE void init_hd();
PRIVATE void hd_identify(int drive);
PRIVATE void hd_cmd_out(struct hd_cmd *cmd);
PRIVATE void interrupt_wait();
PRIVATE void print_identify_info(u16* hd_info);
PRIVATE int waitfor(int mask, int val, int timeout);

PRIVATE u8 hd_status;
PRIVATE u8 hd_buf[SECTOR_SIZE * 2];

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
			hd_identify(0);
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
	printf("NrDrives:%d.\n", *pNrDrives);
	assert(*pNrDrives);
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
	print_identify_info(hd_buf);
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

PRIVATE void print_identify_info(u16* hd_info)
{
	int j = 0;
	char s[64];
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
			s[j * 2 + 1] = *p++;
			s[j * 2] = *p++;
		}
		s[j * 2] = 0;
		printf("%s: %s\n", iinfo[i].desc, s);
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