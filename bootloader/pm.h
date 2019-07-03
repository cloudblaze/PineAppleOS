#ifndef PM_H

#include <stdbool.h>
#include <stdint.h>

#include "config/memory_layout.h"

#define SECTOR_SIZE 512

static inline uint32_t linear_address_to_logical_address(uint32_t linear_address)
{
	return linear_address + UINT32_MAX + 1 - DEFAULT_LOADER_SEGMENT;
}

static inline uint32_t logical_address_to_linear_address(uint32_t logical_address)
{
	return logical_address + DEFAULT_LOADER_SEGMENT;
}

static inline uint32_t linear_address_to_physical_address(uint32_t linear_address)
{
	return linear_address;
}

static inline uint32_t physical_address_to_linear_address(uint32_t physical_address)
{
	return physical_address;
}

static inline uint32_t logical_address_to_physical_address(uint32_t logical_address)
{
	return logical_address_to_linear_address(linear_address_to_physical_address(logical_address));
}

static inline uint32_t physical_address_to_logical_address(uint32_t physical_address)
{
	return physical_address_to_linear_address(linear_address_to_logical_address(physical_address));
}

static inline void io_outb(uint16_t port, uint8_t value)
{
	__asm__(
		"out %%al, %%dx"
		::"a"(value), "d"(port)
	);
}

static inline void io_outb_2(uint16_t port, uint16_t value)
{
	__asm__(
		"out %%al, %%dx\n\t"
		"xchg %%ah, %%al\n\t"
		"out %%al, %%dx"
		::"a"(value), "d"(port)
	);
}

static inline void io_outb_index(uint16_t port_address, uint16_t port_data, uint8_t index, uint8_t value)
{
	__asm__(
		"mov %0, %%dx\n\r"
		"mov %2, %%al\n\r"
		"out %%al, %%dx\n\r"
		"mov %1, %%dx\n\r"
		"mov %3, %%al\n\r"
		"out %%al, %%dx"
		:: "m"(port_address), "m"(port_data), "m"(index), "m"(value)
		: "al", "dx");
}

static inline void io_outw(uint16_t port, uint16_t value)
{
	__asm__(
		"out %ax, %dx"
		:: "a"(value), "d"(port));
}

static inline uint32_t io_inb(uint32_t port)
{
	uint8_t value;
		__asm__(
			"in %%dx, %%al"
			: "=a"(value)
			: "d"(port));
	return value;
}

static inline uint32_t io_inb_index(uint16_t port_address, uint16_t port_data, uint8_t index)
{
	uint8_t value;
	__asm__(
		"mov %1, %%dx\n\r"
		"mov %3, %%al\n\r"
		"out %%al, %%dx\n\r"
		"mov %2, %%dx\n\r"
		"in %%dx, %%al"
		: "=a"(value)
		: "m"(port_address), "m"(port_data), "m"(index)
		: "dx");
	return value;
}

static inline uint32_t io_inw(uint32_t port)
{
	uint16_t value;
		__asm__(
			"in %%dx, %%ax"
			: "=a"(value)
			: "d"(port));
	return value;
}

static inline void sti(void)
{
	__asm__("sti");
}

static inline void cli(void)
{
	__asm__("cli");
}

static inline void std(void)
{
	__asm__("std");
}

static inline void cld(void)
{
	__asm__("cld");
}

static inline void nop(void)
{
	__asm__("nop");
}

// 数据寄存器
#define HD_REGISTER_DATA		0x1f0
// 错误寄存器（错误状态）
#define HD_REGISTER_ERROR	0x1f1
// 扇区数寄存器
#define HD_REGISTER_SECTOR_COUNT 0x1f2
// 起始扇区号0~7位
#define HD_REGISTER_LBA7_0 0x1f3
// 起始扇区号8~15位
#define HD_REGISTER_LBA15_8 0x1f4
// 起始扇区号16~23位
#define HD_REGISTER_LBA23_16 0x1f5
// 起始扇区号24~27位、主从盘
#define HD_REGISTER_LBA27_24 0x1f6
#define HD_REGISTER_SELECTOR 0x1f6
// 状态寄存器
#define HD_REGISTER_STATUS 0x1f7
// 命令寄存器 
#define HD_REGISTER_COMMAND 0x1f7
// 硬盘控制寄存器
#define HD_REGISTER_CONTROL 0x3f6

// 主盘
#define HD_MASTER 0
// 从盘
#define HD_SLAVE 1

// 命令执行错误。
// 	当该位置位时说明前一个命令以出错结束。此时出错寄存器和状态寄存器中的位含有
// 引起错误的一些信息。
#define HD_STATUS_ERROR 0x01
// 收到索引。
//	当磁盘旋转遇到索引标志时会设置该位。
#define HD_STATUS_INDEX 0x02
// ECC校验错。
//	当遇到一个可恢复的数据错误而且已得到纠正，就会设置该位。这种情况不会中断一
// 个多扇区读操作。
#define HD_STATUS_ECC 0x04
// 数据请求服务。
//	当该位被置位时，表示驱动器已经准备好在主机和数据端口之间传输一个字或字节
// 的数据。
#define HD_STATUS_DRQ 0x08
// 驱动器寻道结束。
// 	当该位被置位时，表示寻道操作已经完成，磁头已经停在指定的磁道上。当发生错误时，
// 该位不会改变。只有主机读取了状态寄存器后，该位才会再次表示当前寻道的完成状态。
#define HD_STATUS_SEEK 0x10
// 驱动器故障（写出错）。
// 	当发送错误时，该位并不会改变。只有主机读取了状态寄存器后，该位才会再次表示
// 当前写操作的出错状态。
#define HD_STATUS_WRERR 0x20
// 驱动器准备好（就绪）。
// 	表示驱动器已经准备好接受命令。当发生错误时，该位并不会改变。只有主机读取了
// 状态寄存器后，该位才会再次表示当前驱动器就绪状态。在开机时，应该复位该位，
// 直到驱动器速度达到正常并且能够接收命令。
#define HD_STATUS_READY 0x40
// 驱动器忙碌。
//	当驱动器正在操作由驱动器的控制器设置该位。此时主机不能发送命令。而对任何命令
// 寄存器的读操作将返回状态寄存器的值。在下列条件下该位会被置位：
//	（1）在机器复位信号RESET变负或者设备控制寄存器的SRST被设置之后400纳秒以内。
// 在机器复位之欧要求该位置位状态不能超过30秒。
// 	（2）主机向命令寄存器写重新校正、读、读缓冲、初始化驱动器参数以及执行诊断等
// 命令的400纳秒以内。
//	（3）在写操作、写缓冲或格式化磁道命令期间传输了512字节数据的5微妙之内。
#define HD_STATUS_BUSY 0x80

#define HD_COMMAND_RESTORE 0x10
#define HD_COMMAND_READ 0x20
#define HD_COMMAND_WRITE 0x30
#define HD_COMMAND_VERIFY 0x40
#define HD_COMMAND_FORMAT 0x50
#define HD_COMMAND_INIT 0x60
#define HD_COMMAND_SEEK 0x70
#define HD_COMMAND_DIAGNOSE 0x90
#define HD_COMMAND_SPECIFY 0x91

#define ATA_TYPE_UNKNOWN 0x00
#define ATA_TYPE_PATAPI 0x01
#define ATA_TYPE_SATAPI 0x02
#define ATA_TYPE_PATA 0x03
#define ATA_TYPE_SATA 0x04

#endif