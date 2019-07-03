/*
 * /bootloader/main32.c
 * This file is part of PineAppleOS
 *
 * Copyright (C) 2019 - Huo Yun (cloudblaze@yeah.net)
 *
 * PineAppleOS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PineAppleOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PineAppleOS. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Created by Huo Yun (cloudblaze@yeah.net) - 2019-06-19
 * Description: 本程序是loader加载程序的32位模块，主要功能是在32位保护模式下
 *  将系统内核映像加载到内存的指定位置。
 */

#include <stdbool.h>

#include "pm.h"

static bool hd_controller_ready(void)
{
	int try_times = 100000;
	
	while(--try_times && (io_inb(HD_REGISTER_STATUS) & 0x80))
		;
	
	return try_times > 0;
}

static int hd_detect_device_type(int slave_bit)
{
	int result = 0;
	uint16_t word = 0;

	if(!hd_controller_ready())
	{
		return -1;
	}

	io_outb(HD_REGISTER_SELECTOR, 0xe0 | slave_bit << 4);
	io_inb(HD_REGISTER_CONTROL);
	io_inb(HD_REGISTER_CONTROL);
	io_inb(HD_REGISTER_CONTROL);
	io_inb(HD_REGISTER_CONTROL);

	word = (io_inb(HD_REGISTER_LBA23_16) << 8) + io_inb(HD_REGISTER_LBA15_8);
	switch(word)
	{
		case 0xeb14:
			result = ATA_TYPE_PATAPI;
			break;
		case 0x9669:
			result = ATA_TYPE_SATAPI;
			break;
		case 0:
			result = ATA_TYPE_PATA;
			break;
		case 0xc33c:
			result = ATA_TYPE_SATA;
			break;
		default:
			result = ATA_TYPE_UNKNOWN;
			break;
	}
	
	return result;
}

bool hd_read_sectors(int slave_bit, uint32_t lba_num, uint8_t count, uint32_t physical_address)
{
	uint16_t * buf = (uint16_t *)physical_address_to_logical_address(physical_address_to_linear_address(physical_address));

	if(!hd_controller_ready())
	{
		return false;
	}

	io_outb(HD_REGISTER_LBA27_24, (0xe0 | slave_bit << 4) | ((lba_num >> 24) & 0x0f));
	io_outb(HD_REGISTER_SECTOR_COUNT, count);
	io_outb(HD_REGISTER_LBA7_0, lba_num & 0xff);
	io_outb(HD_REGISTER_LBA15_8, (lba_num >> 8) & 0xff);
	io_outb(HD_REGISTER_LBA23_16, (lba_num >> 16) & 0xff);
	io_outb(HD_REGISTER_COMMAND, HD_COMMAND_READ);

	if(!hd_controller_ready())
	{
		return false;
	}

	for(int i = 0; i < count; i++)
	{
		for(int j = 0; j < SECTOR_SIZE / 2; j++)
		{
			*buf++ = io_inw(HD_REGISTER_DATA);
		}
	}

	return true;
}

void main32(void)
{
	hd_detect_device_type(HD_MASTER);
	hd_read_sectors(HD_MASTER, 0, 1, 0x100000);
}