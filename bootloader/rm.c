/*
 * /bootloader/rm.c
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
 * Description: 与实模式有关的函数
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "rm.h"

void init_regs(biosregs_t * regs)
{
    memset(regs, 0, sizeof(biosregs_t));
    regs->eflags |= EFLAGS_CF;
    regs->ds = ds();
    regs->es = es();
    regs->fs = fs();
    regs->gs = gs();
}

vbe_info_block_t vbe_info_block;
uint16_t vbe_mode_count = 0;
uint16_t vbe_mode_list[100];

uint16_t vbe_get_current_mode_val(void)
{

	biosregs_t in_regs;
	biosregs_t out_regs;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ax = 0x4f03;
	intcall(0x10, &in_regs, &out_regs);

	if(out_regs.ah != 0)
	{
		printf("This function is not supported!\n\r");
		return 0xffff;
	}

	return out_regs.bx;
}

void vbe_set_mode(uint16_t mode_val)
{
	vbe_crtc_info_block_t crtc;

	biosregs_t in_regs;
	biosregs_t out_regs;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ax = 0x4f02;
	in_regs.bx = mode_val;
	in_regs.edi = (uint32_t)&crtc;
	intcall(0x10, &in_regs, &out_regs);

	if(out_regs.ah != 0)
	{
		printf("This function is not supported!\n\r");
		return;
	}
}

void vbe_get_mode_info(uint16_t mode_val)
{
	vbe_mode_info_block_t mode_info;

	biosregs_t in_regs;
	biosregs_t out_regs;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ax = 0x4f01;
	in_regs.cx = mode_val;
	in_regs.edi = (uint32_t)&mode_info;
	intcall(0x10, &in_regs, &out_regs);

	if(out_regs.ah != 0)
	{
		printf("This function is not supported!\n\r");
		return;
	}

	printf("%x: %d*%d %d*%d", mode_val, mode_info.x_resolution,mode_info.y_resolution, mode_info.x_char_size, mode_info.y_char_size);
}

void vbe_get_video_mode_list(int seg, int offs)
{
	uint16_t mode_val;

	while(true)
	{
		__asm__(
			"push %%es\n\t"
			"mov %1, %%es\n\t"
			"movw %%es:(%2), %0\n\t"
			"pop %%es\n\t"
			: "=r"(mode_val)
			: "r"(seg & 0xffff), "r"(offs & 0xffff));
		if(mode_val == 0xffff)
			break;
		
		vbe_mode_list[vbe_mode_count++] = mode_val;
		offs += 2;
	}
}

void vbe_init(void)
{
	biosregs_t in_regs;
	biosregs_t out_regs;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ax = 0x4f00;
	in_regs.edi = (uint32_t)&vbe_info_block;
	intcall(0x10, &in_regs, &out_regs);

	if(out_regs.ah != 0)
	{
		printf("VBE is not supported!\n\r");
		return;
	}

	vbe_get_video_mode_list(vbe_info_block.video_mode_ptr >> 16,
		vbe_info_block.video_mode_ptr & 0xffff);
}

size_t strlen_fptr16(fptr16_t fptr)
{	
	size_t length = 0;

	__asm__(
		"push %%ds\n\t"
		"mov %2, %%ds\n\t"
		"1:lodsb\n\t"
		"test %%al, %%al\n\t"
		"jz 2f\n\t"
		"inc %0\n\t"
		"jmp 1b\n\t"
		"2:pop %%ds"
		: "=r"(length)
		: "0"(0), "r"(get_segment_from_fptr16(fptr)), "S"(get_offset_from_fptr16(fptr))
		: "eax");
	
	return length;
}

void memcpy_fptr16(fptr16_t dest_fptr, fptr16_t src_fptr, size_t n)
{
	__asm__(
		"push %%ds\n\t"
		"push %%es\n\t"
		"mov %1, %%ds\n\t"
		"mov %3, %%es\n\t"
		"cld;rep;movsb;\n\t"
		"pop %%es\n\t"
		"pop %%ds"
		:: "c"(n), "r"(get_segment_from_fptr16(src_fptr)), "S"(get_offset_from_fptr16(src_fptr)), "r"(get_segment_from_fptr16(dest_fptr)), "D"(get_offset_from_fptr16(dest_fptr)));
}

#define _console_print_fptr16 console_print_fptr16
size_t _console_print_fptr16(fptr16_t fptr);

int printf_fptr16(fptr16_t fptr)
{
	return _console_print_fptr16(fptr);
}

bool hd_load_sector_from_disk(uint8_t driver_num, uint32_t lba_start, uint32_t count, fptr16_t buffer)
{
	biosregs_t in_regs;
	biosregs_t out_regs;
	disk_address_packet_t dat;

	init_regs(&in_regs);
	init_regs(&out_regs);
	
	dat.size = 0x10;
	dat.count = count;
	dat.buffer = buffer;
	*(uint32_t *)&dat.start_num = lba_start;

	in_regs.ah = 0x42;
	in_regs.dl = driver_num;
	in_regs.ds = ss();
	in_regs.si = (uint32_t)&dat & 0xffff;
	intcall(0x13, &in_regs, &out_regs);

	if(out_regs.eflags & EFLAGS_CF)
	{
		return false;
	}

	return true;
}