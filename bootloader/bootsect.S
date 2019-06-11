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
BOOTSECT_STACK_SEGMENT = DEF_BOOTSECT_STACK_SEGMENT >> 4
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
	mov $BOOTSECT_STACK_SEGMENT, %ax
	mov %ax, %ss        // 此处使用0x8000~0x8400作为堆栈段
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

load_rest_code:
	pushw $BOOTSECT_SEGMENT
	pushw $0x200
	push boot_device_num
	pushw $1
	pushl $1
	call load_sectors
	add $10, %sp
	test %ax, %ax
	jz load_bootloader

halt1:
	hlt
	jmp halt1

/******************** function load_sectors begin ********************/

/*
 * function: 加载磁盘扇区到内存指定地址。
 * args:
 * 	%bx -- 4(%bp) -- 待加载的起始扇区号
 * 	%cx -- 8(%bp) -- 待加载的扇区数
 *	%dl -- 10(%bp) -- 驱动器号
 * 	%di -- 12(%bp) -- 加载地址偏移量
 * 	%es -- 14(%bp) -- 加载地址段基址
 * return:
 * 	success: %ax == 0
 *	failure: %ax != 0
 */
load_sectors:
	push %bp
	mov %sp, %bp
	push %es
	push %ebx
	push %cx
	push %dx
	push %si
	push %di

	mov 4(%bp), %ebx
	mov 8(%bp), %cx
	mov 10(%bp), %dx
	mov 12(%bp), %di
	pushw 14(%bp)
	pop %es

	movb $0x10, _disk_address_packet_size
	mov %cx, _disk_address_packet_count
	mov %es, %ax
	shl $16, %eax
	mov %di, %ax
	mov %eax, _disk_address_packet_buffer
	mov %ebx, _disk_address_packet_start_num
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
partition_table_entry_1_active:
	.byte 0x80
partition_table_entry_1_start_chs:
	.byte 0x20, 0x21, 0x00
partition_table_entry_1_type:
	.byte 0x0e
partition_table_entry_1_end_chs:
	.byte 0xdf, 0x13, 0x0c
partition_table_entry_1_start_lba:
	.long 0x00000800
partition_table_entry_1_count:
	.long 0x00032000
	
	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_2
partition_table_entry_2:
	// 00 df 14 0c 82 f3 23 10  00 28 03 00 00 00 01 00
	// 206848-272383、共65536个扇区、32M、Linux swap / Solaris
partition_table_entry_2_active:
	.byte 0x00
partition_table_entry_2_start_chs:
	.byte 0xdf, 0x14, 0x0c
partition_table_entry_2_type:
	.byte 0x82
partition_table_entry_2_end_chs:
	.byte 0xf3, 0x23, 0x10
partition_table_entry_2_start_lba:
	.long 0x00032800
partition_table_entry_2_count:
	.long 0x00010000

	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_3
partition_table_entry_3:
	// 00 f3 24 10 81 81 3f 82  00 28 04 00 00 d6 1b 00
	// 272384-2096639、共1824256个扇区、890.8M、Minix / 旧 Linux
partition_table_entry_3_active:
	.byte 0x00
partition_table_entry_3_start_chs:
	.byte 0xf3, 0x24, 0x10
partition_table_entry_3_type:
	.byte 0x81
partition_table_entry_3_end_chs:
	.byte 0x81, 0x3f, 0x82
partition_table_entry_3_start_lba:
	.long 0x00042800
partition_table_entry_3_count:
	.long 0x001bd600
	
	.org BOOTSECT_OFFSECT_PARTITION_TABLE_ENTRY_4
partition_table_entry_4:
	.fill 16, 1, 0

	// 可引导标志
	.org BOOTSECT_OFFSECT_BOOT_FLAG
boot_flag:
	.word 0xAA55

/******************** variable end ********************/

/******************** rest code begin ********************/
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

	// 通过调用子例程load_sectors加载fat16分区文件系统，
	// 并且搜索加载文件bootloader。
	pushw $BOOTLOADER_SEGMENT
	pushw $0
	push boot_device_num
	pushw $1
	pushl partition_table_entry_1_start_lba
	call load_sectors
	add $10, %sp
	test %ax, %ax
	jnz 3f

	// 找到FDT
	pushw $BOOTLOADER_SEGMENT
	pop %gs
	mov %gs:0x0d, %al
	mov %al, sectors_per_cluster
	mov partition_table_entry_1_start_lba, %ebx
	xor %eax, %eax
	mov %gs:0x0e, %ax
	add %eax, %ebx
	xor %eax, %eax
	xor %ecx, %ecx
	mov %gs:0x16, %ax
	mov %gs:0x10, %cl
	mul %cx
	shl $16, %edx
	mov %ax, %dx
	add %edx, %ebx
	mov %ebx, %esi

	mov $0x20, %cl
	div %cl
	mov %al, %cl
	movw %gs:0x11, %ax
	mov %ax, root_directory_entry_count
	div %cl
	mov %al, %dl
	mov %cx, %dx

	// 加载FDT，并且在FDT中搜索文件bootloader。
4:	pushw $BOOTLOADER_SEGMENT
	pushw $0
	push boot_device_num
	pushw $1
	push %esi
	call load_sectors
	add $10, %sp
	test %ax, %ax
	jnz 3f

	mov $0x0b, %edi
2:	cmpb $0x0f, %gs:(%di)
	jne 1f
	add $0x20, %di
	dec %cl
	cmp $0, %cl
	jne 2b
	mov %dx, %cx
	inc %esi
	dec %dl
	jne 4b

1:	pop %cx
	and $0xffffffe0, %edi
	// 此时%gs:0x1a(%edi)为bootloader的起始簇号，%gs:0x1c(%edi)为bootloader的长度

	xor %eax, %eax
	mov root_directory_entry_count, %ax
	shr $4, %ax
	add %eax, %ebx
	xor %edx, %edx
	mov %gs:0x1a(%edi), %cx
	sub $2, %cx
	xor %eax, %eax
	mov sectors_per_cluster, %al
	mul %cx
	shl $16, %edx
	mov %ax, %dx
	add %edx, %ebx

	xor %edx, %edx
	mov %gs:0x1c(%edi), %eax
	add $0x1ff, %eax
	mov $0x200, %cx
	div %cx
	mov %ax, %cx
	
	// 通过调用子例程load_sectors加载bootloader
	pushw $BOOTLOADER_SEGMENT
	pushw $0
	push boot_device_num
	//pushw $1
	//pushw $1
	push %cx
	pushl %ebx
	call load_sectors
	add $10, %sp
	test %ax, %ax
	jnz 3f

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

3:
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
1:xor %dh, %dh

halt2:
	hlt
	jmp halt2

message_loading:
	.asciz "Loading bootloader ... "
message_success:
	.asciz "success!\n\r"
message_failure:
	.asciz "failure!\n\r"

boot_device_num:
	.word 0

sectors_per_cluster:
	.byte 0

root_directory_entry_count:
	.word 0

/******************** rest code end ********************/