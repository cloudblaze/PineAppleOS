/*
 * /include/config/memory_layout.h
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
 * Created by Huo Yun - 2019-06-07
 * Description: 磁盘映像中各个部分加载到物理内存中的布局情况
 */

#ifndef MEMORY_LAYOUT_H
#define MEMORY_LAYOUT_H

#define DEF_BOOTSECT_SEGMENT 0x07c00
#define DEF_BOOTSECT_STACK_SEGMENT 0x08000
#define DEF_BOOTLOADER_SEGMENT 0x10000
#define DEF_BOOTPARAM_SEGMENT 0x90000
#define DEF_VGA_SEGMENT 0xb8000
#define DEF_SYSTEM_PHYSICAL_ADDRESS 0x100000

#define DEF_BOOTLOADER_MAX_LENGTH 0x80000

#endif