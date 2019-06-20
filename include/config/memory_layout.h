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

#define DEFAULT_BOOT_SEGMENT 0x07c00
#define DEFAULT_LOADER_SEGMENT 0x10000
#define DEFAULT_BOOTPARAM_SEGMENT 0x90000
#define DEFAULT_BOOT_STACK_SEGMENT 0x98000
#define DEFAULT_BOOT_BUFFER_SEGMENT (DEFAULT_BOOT_STACK_SEGMENT + DEFAULT_BOOT_STACK_LENGTH)
#define DEFAULT_BOOT_CLUSTER_NUM_SEGMENT (DEFAULT_BOOT_BUFFER_SEGMENT + DEFAULT_BOOT_BUFFER_LENGTH)
#define DEFAULT_VGA_SEGMENT 0xb8000
#define DEFAULT_SYSTEM_PHYSICAL_ADDRESS 0x100000

#define DEFAULT_LOADER_MAX_LENGTH 0x80000
#define DEFAULT_BOOTPARAM_MAX_LENGTH 0x8000
#define DEFAULT_BOOT_STACK_LENGTH 0x400
#define DEFAULT_BOOT_BUFFER_LENGTH 0x200

#endif