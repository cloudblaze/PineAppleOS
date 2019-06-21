/*
 * /bootloader/main16.c
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
 * Description: 本程序是loader加载程序的16位模块，主要功能是初始化程序、获取硬件参数。
 */

#include <stdint.h>
#include <stdio.h>

#include <config/bootsect.h>
#include <config/memory_layout.h>

#include "rm.h"

#define SECTS_PER_TRACK 63
#define HEADS 16

uint16_t ram_size = 0;
uint16_t scr_rows = 25;
uint16_t scr_cols = 80;

uint16_t e820_entry_cnt = 0;
e820_entry_t e820_entry_list[100];
uint16_t hd_info_cnt = 0;
hd_info_t hd_info_list[2];
DPT_t dpt[2];

extern uint16_t vbe_mode_count;
extern uint16_t vbe_mode_list[100];

static uint16_t get_ram_size(void)
{
	biosregs_t in_regs;
	biosregs_t out_regs;
	
	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.eax = 0xe801;
	intcall(0x15, &in_regs, &out_regs);
	if(out_regs.eflags & EFLAGS_CF)
	{
		printf("Function call failed!\n");
		return 0;
	}

	return (out_regs.bx * 64 + out_regs.ax) / 1024 + 1;
}

static uint8_t e820(e820_entry_t * e820_entry_list)
{
	uint8_t e820_entry_cnt = 0;
	e820_entry_t e820_entry;
	biosregs_t in_regs;
	biosregs_t out_regs;
	
	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ebx = 0;
	in_regs.edi = (uint32_t)&e820_entry;
	
	do
	{
		in_regs.eax = 0xe820;
		in_regs.ecx = sizeof(e820_entry);
		in_regs.edx = 0x534d4150;
		intcall(0x15, &in_regs, &out_regs);
		if(out_regs.eflags & EFLAGS_CF)
		{
			printf("Function call failed!\n");
			return 0;
		}
		
		in_regs.ebx = out_regs.ebx;
		e820_entry_list[e820_entry_cnt++] = e820_entry;
	} while(out_regs.ebx != 0);

	return e820_entry_cnt;
}

static void get_hd_info(hd_info_t hd_info_list[2])
{
	fptr16_t dest_fptr;
	fptr16_t src_fptr;

	dest_fptr = get_fptr16_from_logic_address(ds(), (uint32_t)&hd_info_list[0]);
	src_fptr = get_value_by_fptr16(get_fptr16_from_phys_address(0x41 * 4), fptr16_t);
	memcpy_fptr16(dest_fptr, src_fptr, sizeof(hd_info_t));
	if(hd_info_list[0].cyl != 0 && hd_info_list[0].head != 0 && hd_info_list[0].sect != 0)
		hd_info_cnt++;

	dest_fptr = get_fptr16_from_logic_address(ds(), (uint32_t)&hd_info_list[1]);
	src_fptr = get_value_by_fptr16(get_fptr16_from_phys_address(0x46 * 4), fptr16_t);
	memcpy_fptr16(dest_fptr, src_fptr, sizeof(hd_info_t));
	if(hd_info_list[1].cyl != 0 && hd_info_list[1].head != 0 && hd_info_list[1].sect != 0)
		hd_info_cnt++;
}

void main16(void)
{
	for(int i = 0; i < scr_cols; i++)
		printf("-");
	printf("Now, we could use C environment. :)\n");
	printf("System Infomation:\n");

	vbe_init();
	printf("  Vga Mode: %d, Screen Rows: %d, Screen Cols: %d\n", vbe_get_current_mode_val(), scr_rows, scr_cols);
	printf("  Vbe Supported Mode Num:\n");
	for(int i = 0; i < vbe_mode_count; i += 5)
	{
		printf("    %#04x    %#06x    %#06x    %#06x    %#06x\n", vbe_mode_list[i], vbe_mode_list[i + 1], vbe_mode_list[i + 2], vbe_mode_list[i + 3], vbe_mode_list[i + 4]);
	}

	ram_size = get_ram_size();
	printf("  Ram Size: %d MB\n", ram_size);

	e820_entry_cnt = e820(e820_entry_list);
	printf("  Memory Map:\n");
	for(int i = 0; i < e820_entry_cnt; i++)
	{
		uint64_t end_addr =e820_entry_list[i].address + e820_entry_list[i].length;
		printf("    %#018llx - %#018llx: ", e820_entry_list[i].address, end_addr);
		switch(e820_entry_list[i].type)
		{
			case 1:
				printf("Available Memory\n");
				break;
			case 2:
				printf("Reserved\n");
				break;
			case 3:
				printf("ACPI Reclaim Memory\n");
				break;
			case 4:
				printf("ACPI NVS Memory\n");
				break;
			default:
				printf("Unknown Type\n");
				break;
		}
	}
	
	get_hd_info(hd_info_list);
	printf("Hard disk:\n");
	for(int i = 0; i < hd_info_cnt; i++)
	{
		fptr16_t src_ptr = get_fptr16_from_logic_address(DEFAULT_BOOT_SEGMENT >> 4, BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_1);
		fptr16_t dest_ptr = get_fptr16_from_logic_address(ds(), (uint32_t)&dpt[i]);
		memcpy_fptr16(dest_ptr, src_ptr, sizeof(DPT_t));

		printf("  sd%c: total %d sectors\n", 'a' + i, hd_info_list[i].cyl * hd_info_list[i].head * hd_info_list[i].sect);
		for(int j = 0; j < 4; j++)
		{
			if(dpt[i].parts[j].lba_length > 0)
			{
				printf("    sd%c%d: %d - %d, total %d sectors\n", 'a' + i, j + 1,
					dpt[i].parts[j].lba_start,
					dpt[i].parts[j].lba_start + dpt[i].parts[j].lba_length - 1,
					dpt[i].parts[j].lba_length);
			}
		}
	}

	printf("Now, we will enter protected mode and load system image.\n");
}