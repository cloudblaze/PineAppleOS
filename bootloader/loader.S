/*
 * /bootloader/loader.S
 * This file is part of PineAppleOS
 *
 * Copyright (C) 2019 - Huo Yun
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
 * Created by Huo Yun - 2019-06-06
 * Description: 加载程序loader是bootloader的第二部分，而本程序是loader加载程序的入口模块，
 *	在该模块中创建C语言执行环境。
 */

#include <config/memory_layout.h>

	.code16
	.section .text
	.global _start

_start:
	mov %cs, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss	// 在GCC中假定的是DS段和SS段是同一个段
	xor %bp, %bp
	mov %bp, %sp
	
	calll main16

	calll main32

halt:
	hlt
	jmp halt