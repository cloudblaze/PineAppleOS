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

uint16_t scr_rows = 25;
uint16_t scr_cols = 80;

void main16(void)
{
	for(int i = 0; i < scr_cols; i++)
		printf("-");
	printf("Now, we could use C environment. :)\n");
	printf("System Infomation:\n");
}