/*
 * /bootloader/boot.S
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
 * Description: 该程序是bootloader的第一部分，也就是自举程序boot，其中包含磁盘引导扇区。
 * 	由于磁盘引导扇区的容量不够，故将一部分代码放置在磁盘后续的扇区上。引导扇区先将后续扇区
 * 	加载进内存中，然后再对FAT16文件系统进行解析，最终找到bootloader的第二部分（加载程序loader）。
 */

#include <config/bootsect.h>
#include <config/boot_parameter.h>
#include <config/memory_layout.h>

#define DEFAULT_LOADER_FILENAME "loader"

BOOT_SEGMENT = DEFAULT_BOOT_SEGMENT >> 4
LOADER_SEGMENT = DEFAULT_LOADER_SEGMENT >> 4
BOOTPARAM_SEGMENT = DEFAULT_BOOTPARAM_SEGMENT >> 4
BOOT_STACK_SEGMENT = DEFAULT_BOOT_STACK_SEGMENT >> 4
BOOT_BUFFER_SEGMENT = DEFAULT_BOOT_BUFFER_SEGMENT >> 4
BOOT_CLUSTER_NUM_SEGMENT = DEFAULT_BOOT_CLUSTER_NUM_SEGMENT >> 4

BOOT_STACK_LENGTH = DEFAULT_BOOT_STACK_LENGTH

DEFAULT_CHARACTER_ATTRIBUTE = 0x07

// 每扇区字节数
FAT16_DBP_OFFSET_BYTES_PER_SECTOR = 0x0b
// 每簇扇区数
FAT16_DBP_OFFSET_SECTORS_PER_CLUSTER = 0x0d
// 保留扇区的数目
FAT16_DBP_OFFSET_RESERVED_SECTORS = 0x0e
// FAT表数目
FAT16_DBP_OFFSET_FAT_COUNT = 0x10
// 在根目录中可容纳的目录项个数。需要注意的是，由于有长文件名的存在，目录项个数不等于文件个数。
FAT16_DBP_OFFSET_ROOT_DIRECTORY_ENTRY_COUNT = 0x11
// 一个FAT表所占用的扇区数
FAT16_DBP_OFFSET_SECTORS_PER_FAT = 0x16

// 文件属性
FAT16_FDE_OFFSET_MODE = 0x0b
// 文件的起始簇号
FAT16_FDE_OFFSET_START_CLUSTER_NUM = 0x1a
// 文件大小，以字节为单位
FAT16_FDE_OFFSET_LENGTH = 0x1c

// FDT表中目录项的大小
FAT16_DIRECTORY_ENTRY_SIZE = 0x20

// FDT表中长文件名的目录项属性
FAT16_LONG_FILENAME_MODE = 0x0f
FAT16_FILE_DELETED_FLAG = 0xe5

// FAT表中的结束标志
FAT16_FAT_END_FLAG = 0xffff

DEFAULT_BYTES_PER_SECTOR = 0x200

	.code16
	.section .text
	.global _start
	.global LABEL_debug

_start:
	ljmp $BOOT_SEGMENT, $1f
1:

	// 初始化段寄存器
	mov %cs, %ax
	mov %ax, %ds
	mov $BOOT_STACK_SEGMENT, %ax
	mov %ax, %ss
	mov $BOOT_STACK_LENGTH, %bp
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

	// 加载boot程序剩余代码
load_rest_code:
	mov boot_len, %ax
	add $DEFAULT_BYTES_PER_SECTOR, %ax
	dec %ax
	xor %dx, %dx
	mov $DEFAULT_BYTES_PER_SECTOR, %cx
	div %cx
	dec %ax

	pushw $BOOT_SEGMENT
	pushw $0x200
	pushw boot_device_num
	pushw %ax
	pushl $1
	call load_sectors
	add $12, %sp
	test %ax, %ax
	jz load_loader

	// 如果boot程序剩余代码加载失败，则停机
halt1:
	hlt
	jmp halt1

/******************** function load_sectors begin ********************/

/*
 * function: 加载磁盘扇区到内存指定地址。
 * args:
 * 	%ebx -- 4(%bp) -- 待加载的起始扇区号
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
	mov $_disk_address_packet, %si
	int $0x13
	
	pop %di
	pop %si
	pop %dx
	pop %cx
	pop %ebx
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

boot_device_num:
	.word 0
boot_len:
	.word boot_end

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
	// 加载loader
load_loader:
	// 屏幕打印"Loading loader ... "
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
	// 并且搜索加载文件loader。
	pushw $BOOT_BUFFER_SEGMENT
	pushw $0
	pushw boot_device_num
	pushw $1
	pushl partition_table_entry_1_start_lba
	call load_sectors
	add $12, %sp
	test %ax, %ax
	jnz load_sector_failure

	// 找到FDT
	pushw $BOOT_BUFFER_SEGMENT
	pop %gs
	mov %gs:FAT16_DBP_OFFSET_SECTORS_PER_CLUSTER, %al
	mov %al, sectors_per_cluster
	mov partition_table_entry_1_start_lba, %ebx
	movzxw %gs:FAT16_DBP_OFFSET_RESERVED_SECTORS, %eax
	add %eax, %ebx
	mov %ebx, fat_start_num
	mov %gs:FAT16_DBP_OFFSET_SECTORS_PER_FAT, %ax
	movzxb %gs:FAT16_DBP_OFFSET_FAT_COUNT, %cx
	mul %cx
	shl $16, %edx
	mov %ax, %dx
	add %edx, %ebx
	mov %ebx, fdt_start_num
	mov %ebx, %esi

	mov %gs:FAT16_DBP_OFFSET_BYTES_PER_SECTOR, %ax
	mov %ax, bytes_per_sector
	movb $FAT16_DIRECTORY_ENTRY_SIZE, %cl
	div %cl
	mov %al, %cl	// %cl -- 每个扇区包含的目录项个数
	mov %cl, _rest_fde_count_of_current_sector
	mov %gs:FAT16_DBP_OFFSET_ROOT_DIRECTORY_ENTRY_COUNT, %ax
	mov %ax, root_directory_entry_count
	div %cl
	mov %al, %dl	// %dl -- 保存所有根目录项所需的扇区个数

	// 加载FDT，并且在FDT中搜索文件loader。
4:	pushw $BOOT_BUFFER_SEGMENT
	pushw $0
	pushw boot_device_num
	pushw $1
	push %esi
	call load_sectors
	add $12, %sp
	test %ax, %ax
	jnz load_sector_failure

	mov $FAT16_FDE_OFFSET_MODE, %di
2:	cmpb $FAT16_LONG_FILENAME_MODE, %gs:(%di)
	je 1f
	cmpb $FAT16_FILE_DELETED_FLAG, %gs:(%di)
	je 1f
	// 按长文件名确定是否为指定文件的目录项
	mov %di, %ax
	and $0xffe0, %ax
	push %gs
	push %ax
	call check_fde_by_long_filename
	add $4, %sp
	test %ax, %ax
	jnz 1f
	jmp 3f
1:	add $FAT16_DIRECTORY_ENTRY_SIZE, %di
	dec %cl
	cmp $0, %cl
	jne 2b
	mov _rest_fde_count_of_current_sector, %cl
	inc %esi
	dec %dl
	cmp $0, %dl
	je load_sector_failure
	jmp 4b

3:	and $0xffe0, %di
	// 此时，%gs:FAT16_FDE_OFFSET_START_CLUSTER_NUM(%di)为loader的起始簇号，
	// 而，%gs:FAT16_FDE_OFFSET_LENGTH(%di)为loader的长度
	mov %gs:FAT16_FDE_OFFSET_START_CLUSTER_NUM(%di), %ax
	mov %ax, loader_start_cluster_num

	// 获取文件loader所占用的簇号
	pushw $BOOT_CLUSTER_NUM_SEGMENT
	pop %es
	xor %di, %di
	stosw
	incw loader_cluster_count
	mov %esi, last_sector_num

2:	mov %es:-2(%di), %si
	mov $DEFAULT_BYTES_PER_SECTOR, %cx
	shr $1, %cx
	xor %dx, %dx
	mov %si, %ax
	div %cx
	movzxw %ax, %eax
	add fat_start_num, %eax
	cmp %eax, last_sector_num
	je 1f

	mov %eax, last_sector_num
	pushw $BOOT_BUFFER_SEGMENT
	pushw $0
	pushw boot_device_num
	pushw $1
	pushl %eax
	call load_sectors
	add $12, %sp
	test %ax, %ax
	jnz load_sector_failure

1:	mov %dx, %si
	shl $1, %si
	cmpw $FAT16_FAT_END_FLAG, %gs:(%si)
	je 1f
	mov %gs:(%si), %ax
	stosw
	incw loader_cluster_count
	jmp 2b

	// 跳过FDT表
1:	mov $FAT16_DIRECTORY_ENTRY_SIZE, %ax
	mulw root_directory_entry_count
	divw bytes_per_sector
	and $0x0000ffff, %eax
	add %eax, %ebx
	mov %ebx, data_start_num

	// 通过调用子例程load_sectors_by_cluster_num加载loader
	xor %si, %si
	mov $LOADER_SEGMENT, %dx
	mov bytes_per_sector, %ax
	shr $4, %ax
	mulb sectors_per_cluster
	mov %ax, %bx
	mov loader_cluster_count, %cx
2:	mov %es:(%si), %ax

	push %dx
	pushw $0
	pushw boot_device_num
	push %ax
	call load_sectors_by_cluster_num
	add $8, %sp
	test %ax, %ax
	jnz load_sector_failure
	add $2, %si
	add %bx, %dx
	loop 2b
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

	// 跳转到loader
	ljmp $LOADER_SEGMENT, $0

load_sector_failure:
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

/******************** function load_sectors_by_cluster_num begin ********************/

/*
 * function: 将FAT16分区中指定簇号的扇区加载到内存指定地址。
 * args:
 * 	4(%bp) -- 待加载的簇号
 *	6(%bp) -- 驱动器号
 * 	8(%bp) -- 加载地址偏移量
 * 	10(%bp) -- 加载地址段基址
 * return:
 * 	success: %ax == 0
 *	failure: %ax != 0
 */
load_sectors_by_cluster_num:
	push %bp
	mov %sp, %bp
	push %cx
	push %edx

	movzxb sectors_per_cluster, %ax
	mov 4(%bp), %cx
	sub $2, %cx
	mul %cx
	shl $16, %edx
	mov %ax, %dx
	add data_start_num, %edx

	// 通过调用子例程load_sectors加载指定簇中的扇区
	pushw 10(%bp)
	pushw 8(%bp)
	pushw 6(%bp)
	pushw sectors_per_cluster
	pushl %edx
	call load_sectors
	add $12, %sp

	pop %edx
	pop %cx
	mov %bp, %sp
	pop %bp
	ret

data_start_num:
	.long 0

/******************** function load_sectors_by_cluster_num end ********************/

/******************** function check_fde_by_long_filename begin ********************/

/*
 * function: 将FAT16分区中指定簇号的扇区加载到内存指定地址。
 * args:
 * 	4(%bp) -- 加载地址偏移量
 * 	6(%bp) -- 加载地址段基址
 * return:
 * 	success: %ax == 0
 *	failure: %ax != 0
 */
check_fde_by_long_filename:
	push %bp
	mov %sp, %bp
	push %gs
	push %di

	pushw 6(%bp)
	pop %gs
	mov 4(%bp), %di

	xor %ax, %ax
	//mov $1, %ax

	pop %di
	pop %gs
	mov %bp, %sp
	pop %bp
	ret

/******************** function check_fde_by_long_filename end ********************/

message_loading:
	.asciz "Loading loader ... "
message_success:
	.asciz "success!\n\r"
message_failure:
	.asciz "failure!\n\r"

bytes_per_sector:
	.word 0
sectors_per_cluster:
	.byte 0
root_directory_entry_count:
	.word 0

fat_start_num:
	.long 0
fdt_start_num:
	.long 0

loader_start_cluster_num:
	.word 0
loader_cluster_count:
	.word 0
loader_filename:
	.asciz DEFAULT_LOADER_FILENAME

last_sector_num:
	.long 0

_rest_fde_count_of_current_sector:
	.byte 0

/******************** rest code end ********************/

boot_end:

	.align 0x200
	/*
	 * 由于按照512字节（一个扇区的大小）对齐，故gas会在此处填充一些数据，如下：
 3f6:	8d b4 00 00          	lea    0x0(%si),%si
 3fa:	8d b4 00 00          	lea    0x0(%si),%si
 3fe:	66 90                	xchg   %eax,%eax
	 */