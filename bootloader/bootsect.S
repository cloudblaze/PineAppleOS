/*
 * /bootloader/bootsect.S
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
 * Description: 磁盘引导扇区，同时也是bootloader的第一部分。
 */

#include <config/bootsect.h>
#include <config/boot_parameter.h>
#include <config/memory_layout.h>

BOOTSECT_SEGMENT = DEF_BOOTSECT_SEGMENT >> 4
BOOTLOADER_SEGMENT = DEF_BOOTLOADER_SEGMENT >> 4
BOOTPARAM_SEGMENT = DEF_BOOTPARAM_SEGMENT >> 4

DEFAULT_CHARACTER_ATTRIBUTE = 0x07

	.code16
	.section .text
	.global _start

_start:
	ljmp $BOOTSECT_SEGMENT, $1f
1:

	// 初始化段寄存器
	mov %cs, %ax
	mov %ax, %ds
	mov %ax, %ss        // 此处使用0x7c00~0x8000作为堆栈段
	mov $0x400, %bp
	mov %bp, %sp

	// 保存启动设备驱动器号
	mov $BOOTPARAM_SEGMENT, %ax
	mov %ax, %gs    
	xor %dh, %dh
	mov %dx, boot_device_num
	mov %dx, %gs:BOOT_DEVICE_NUM

	// 设置VGA为模式3，80*25，字符模式
	mov $0x0003, %ax
	int $0x10

	// 加载bootloader
load_bootloader:
	// 屏幕打印"Loading bootloader ... "
	xor %bh, %bh
	mov $DEFAULT_CHARACTER_ATTRIBUTE, %bl
	mov $message_loading, %si
2:	lodsb
	test %al, %al
	je 1f
	mov $0x0e, %ah
	int $0x10
	jmp 2b
1:

	// 通过调用子例程load_sectors加载bootloader
	pushw $BOOTLOADER_SEGMENT
	pushw $0
	push boot_device_num
	pushw $1
	pushw $1
	call load_sectors
	add $10, %sp
	test %ax, %ax
	jz 3f

	// 屏幕打印"failure!"
	xor %bh, %bh
	mov $DEFAULT_CHARACTER_ATTRIBUTE, %bl
	mov $message_failure, %si
2:	lodsb
	test %al, %al
	je 1f
	mov $0x0e, %ah
	int $0x10
	jmp 2b
1:

halt:
	hlt
	jmp halt

3:
	// 屏幕打印"success!"
	xor %bh, %bh
	mov $DEFAULT_CHARACTER_ATTRIBUTE, %bl
	mov $message_success, %si
2:	lodsb
	test %al, %al
	je 1f
	mov $0x0e, %ah
	int $0x10
	jmp 2b
1:

	// 跳转到bootloader
	ljmp $BOOTLOADER_SEGMENT, $0

/******************** function load_sectors begin ********************/

/*
 * function: 加载磁盘扇区到内存指定地址。
 * args:
 * 	%bx -- 4(%bp) -- 待加载的起始扇区号
 * 	%cx -- 6(%bp) -- 待加载的扇区数
 *	%dl -- 8(%bp) -- 驱动器号
 * 	%di -- 10(%bp) -- 加载地址偏移量
 * 	%es -- 12(%bp) -- 加载地址段基址
 * return:
 * 	success: %ax == 0
 *	failure: %ax != 0
 */
load_sectors:
	push %bp
	mov %sp, %bp
	push %es
	push %bx
	push %cx
	push %dx
	push %si
	push %di

	mov 4(%bp), %bx
	mov 6(%bp), %cx
	mov 8(%bp), %dx
	mov 10(%bp), %di
	pushw 12(%bp)
	pop %es

	movb $0x10, _disk_address_packet_size
	mov %cx, _disk_address_packet_count
	mov %es, %ax
	shl $16, %eax
	mov %di, %ax
	mov %eax, _disk_address_packet_buffer
	mov %bx, _disk_address_packet_start_num
	mov $0x42, %ah
	mov $0x80, %dl
	mov $_disk_address_packet, %si
	int $0x13
	
	pop %di
	pop %si
	pop %dx
	pop %cx
	pop %bx
	pop %es
	mov %bp, %sp
	pop %bp
	ret

_disk_address_packet:
_disk_address_packet_size:			.byte 0
_disk_address_packet_reserved:		.byte 0
_disk_address_packet_count:			.word 0
_disk_address_packet_buffer:		.long 0
_disk_address_packet_start_num:		.quad 0

/******************** function load_sectors end ********************/

message_loading:
	.asciz "Loading bootloader ... "
message_success:
	.asciz "success!\n\r"
message_failure:
	.asciz "failure!\n\r"

boot_device_num:
	.word 0

/******************** variable begin ********************/

	// MBR分区格式DOS磁盘标签
	.org BOOTSECT_OFFSECT_DOS_DISK_LABEL
dos_disk_label:
	.long 0xe24bc5d7

	// MBR分区表
	.org BOOTSECT_OFFSECT_PARTITION_TABLE
	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_1
partition_table_entry_1:
	// 80 20 21 00 0e df 13 0c  00 08 00 00 00 20 03 00
	// 可启动、2048-206847、共204800个扇区、100M、W95 FAT16 (LBA)
	.byte 0x80, 0x20, 0x21, 0x00, 0x0e, 0xdf, 0x13, 0x0c
	.byte 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x03, 0x00
	
	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_2
partition_table_entry_2:
	// 00 df 14 0c 82 f3 23 10  00 28 03 00 00 00 01 00
	// 206848-272383、共65536个扇区、32M、Linux swap / Solaris
	.byte 0x00, 0xdf, 0x14, 0x0c, 0x82, 0xf3, 0x23, 0x10
	.byte 0x00, 0x28, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00

	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_3
partition_table_entry_3:
	// 00 f3 24 10 81 81 3f 82  00 28 04 00 00 d6 1b 00
	// 272384-2096639、共1824256个扇区、890.8M、Minix / 旧 Linux
	.byte 0x00, 0xf3, 0x24, 0x10, 0x81, 0x81, 0x3f, 0x82
	.byte 0x00, 0x28, 0x04, 0x00, 0x00, 0xd6, 0x1b, 0x00
	
	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_4
partition_table_entry_4:
	.fill 16, 1, 0

	// 可引导标志
	.org BOOTSECT_OFFSECT_BOOT_FLAG
boot_flag:
	.word 0xAA55

/******************** variable end ********************/