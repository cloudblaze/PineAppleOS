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
 * Description: bootloader的入口模块，创建C语言执行环境。
 */

	.code16
	.section .text
	.global _start

_start:
	mov $0x11223344, %eax

halt:
	hlt
	jmp halt

	.org 0x7fffc
	.long 0x55667788
	.org 0x80000