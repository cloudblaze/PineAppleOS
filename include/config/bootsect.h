/*
 * /include/config/bootsect.h
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
 * Description: 磁盘引导扇区变量相对地址偏移
 */

#ifndef BOOTSECT_H
#define BOOTSECT_H

#define BOOTSECT_OFFSECT_DOS_DISK_LABEL 0x1B8
#define BOOTSECT_OFFSECT_PARTITION_TABLE 0x1BE
#define BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_1 0x1BE
#define BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_2 0x1CE
#define BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_3 0x1DE
#define BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_4 0x1EE
#define BOOTSECT_OFFSECT_BOOT_FLAG 0x1FE

#endif