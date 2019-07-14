/*
 * /bootloader/fat16.h
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
 * Created by Huo Yun (cloudblaze@yeah.net) - 2019-07-05
 * Description: 实现FAT16文件系统的读取，从中加载内核可执行程序system.elf到内存。
 */

#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

typedef struct
{
    // 0x00 跳转指令，指向启动代码
    uint8_t jmp_code[3];
    // 0x03 建议值为"MSWIN4.1"
    uint8_t oem_name[8];
    struct
    {
        // 0x0b 每扇区字节数，可能值为512、1024、2048、4096
        uint16_t bytes_per_sector;
        // 0x0d 每簇的扇区数，其值必须是2的整数次方，同时还要保证每簇的子结束不能超过32KB
        uint8_t sectors_per_cluster;
        // 0x0e 保留扇区的数目，FAT12/FAT16必须为1
        uint16_t reserved_sectors;
        // 0x10 分区中FAT表的份数
        uint8_t fats;
        // 0x11 更目录中目录的个数，经典值是512
        uint16_t directories_of_root;
        // 0x13 早期版本中16bit的总扇区数。
        uint16_t total_secotrs;
        // 0x15 磁盘类型，0xf8为不可移动磁盘，0xf0为可移动磁盘
        uint8_t media_type;
        // 0x16 一个FAT表所占用的扇区数
        uint16_t sectors_per_fat;
        // 0x18 每磁道扇区数
        uint16_t sectors_per_track;
        // 0x1a 磁头数
        uint16_t heads;
        // 0x1c 在此FAT分区之前所隐藏的扇区数
        uint32_t hidden_secotrs;
        // 0x20 该卷总扇区数
        uint32_t sectors_of_partition;
    } bpb;
    // 0x24 磁盘驱动器号
    uint8_t driver_num;
    // 0x25 保留字段
    uint8_t reserved_field1;
    // 0x26 扩展引导标记(0x29)，用于指明此后3个域可用
    uint8_t extend_flag;
    // 0x27 卷标序列号
    uint32_t boot_sign;
    // 0x2b 磁盘卷标，但FAT卷没有卷标时，此域的内容是"NO NAME"
    uint8_t volum_label[11];
    // 0x36 文件系统类型，可能值是"FAT12"、"FAT16"、"FAT32"
    uint8_t filesystem_type[8];
} __attribute__((packed)) fat16_boot_sector_t;

typedef struct
{
    // 0x00 主文件名
    uint8_t master_name[8];
    // 0x08 扩展文件名
    uint8_t extension_name[3];
    // 0x0b 文件属性
    uint8_t mode;
    // 0x0c 保留字段1
    uint8_t reserved_field1;
    // 0x0d 文件创建时间，精确到10MS的值
    uint8_t create_time_10ms;
    // 0x0e 文件创建时间
    uint16_t create_time;
    // 0x10 文件创建日期
    uint16_t create_date;
    // 0x12 文件最近访问日期
    uint16_t accessed_date;
    // 0x14 保留字段2
    uint16_t reserved_field2;
    // 0x16 修改时间
    uint16_t modified_time;
    // 0x18 修改日期
    uint16_t modified_date;
    // 0x1a 文件的起始簇号
    uint16_t start_cluster_num;
    // 0x1c 文件大小（以字节为单位）
    uint32_t length;
} __attribute__((packed)) fat16_fdt_t;

#endif