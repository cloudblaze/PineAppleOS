/*
 * /bootloader/console.c
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
 * Description: 控制台输出程序
 */

#include <stdint.h>
#include <string.h>

#include "rm.h"

uint16_t console_get_cursor_pos(void)
{
	biosregs_t in_regs;
	biosregs_t out_regs;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ah = 0x03;
	in_regs.bh = 0x00;
	intcall(0x10, &in_regs, &out_regs);

	return out_regs.dx;
}

void console_set_cursor_pos(uint16_t pos)
{
	biosregs_t in_regs;

	init_regs(&in_regs);
	in_regs.ah = 0x02;
	in_regs.bh = 0x00;
	in_regs.dx = pos;
	intcall(0x10, &in_regs, NULL);
}

void console_get_any_key(void)
{
	biosregs_t in_regs;

	init_regs(&in_regs);
	in_regs.ah = 0x00;
	intcall(0x16, &in_regs, NULL);
}

void console_scroll_lines(int8_t lines)
{
	biosregs_t in_regs;

	init_regs(&in_regs);
	if(lines > 0)
	{
		in_regs.ah = 0x06;
		in_regs.al = lines;
	}
	else if(lines < 0)
	{
		in_regs.ah = 0x07;
		in_regs.al = -lines;
	}

	in_regs.bh = 0;
	in_regs.cx = 0x0000;
	in_regs.dx = 0x1950;
	intcall(0x10, &in_regs, NULL);

	console_set_cursor_pos(console_get_cursor_pos() - 0x0100);
}

void console_print(const char * str)
{
	biosregs_t in_regs;
	const char * p = str;

	init_regs(&in_regs);
	in_regs.ah = 0x0e;
	while(*p)		
	{
		in_regs.al = *p;
		intcall(0x10, &in_regs, NULL);
		p++;
	}
}

size_t console_print_fptr16(fptr16_t fptr)
{
	biosregs_t in_regs;
	biosregs_t out_regs;
	size_t len = 0;

	init_regs(&in_regs);
	init_regs(&out_regs);
	in_regs.ah = 0x03;
	in_regs.bh = 0;
	intcall(0x10, &in_regs, &out_regs);

	len = strlen_fptr16(fptr);
	init_regs(&in_regs);
	in_regs.ax = 0x1301;
	in_regs.bx = 0x0007;
	in_regs.cx = len;
	in_regs.dh = out_regs.dh;
	in_regs.dl = out_regs.dl;
	in_regs.bp = get_offset_from_fptr16(fptr);
	in_regs.es = get_segment_from_fptr16(fptr);
	intcall(0x10, &in_regs, NULL);

	return len;
}