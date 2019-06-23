/*
 * /bootloader/rm.h
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
 * Created by Huo Yun (cloudblaze@yeah.net) - 2019-06-19
 * Description: 与实模式相关的头文件
 */

#ifndef RM_H
#define RM_H

__asm__(".code16gcc");

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static inline uint16_t cs(void)
{
	uint16_t value;
	__asm__(
		"mov %%cs, %0"
		: "=r"(value));
	return value;
}

static inline uint16_t ds(void)
{
	uint16_t value;
	__asm__(
		"mov %%ds, %0"
		: "=r"(value));
	return value;
}

static inline void set_ds(uint16_t value)
{
	__asm__("mov %0, %%ds"::"r"((value) & 0xffff));
}

static inline uint16_t es(void)
{
	uint16_t value;
	__asm__(
		"mov %%es, %0"
		: "=r"(value));
	return value;
}

static inline void set_es(uint16_t value)
{
	__asm__("mov %0, %%es"::"r"((value) & 0xffff));
}

static inline uint16_t fs(void)
{
	uint16_t value;
	__asm__(
		"mov %%fs, %0"
		: "=r"(value));
	return value;
}

static inline void set_fs(uint16_t value)
{
	__asm__("mov %0, %%fs"::"r"((value) & 0xffff));
}

static inline uint16_t gs(void)
{
	uint16_t value;
	__asm__(
		"mov %%gs, %0"
		: "=r"(value));
	return value;
}

static inline void set_gs(uint16_t value)
{
	__asm__("mov %0, %%gs"::"r"((value) & 0xffff));
}

static inline uint16_t ss(void)
{
	uint16_t value;
	__asm__(
		"mov %%ss, %0"
		: "=r"(value));
	return value;
}

typedef struct
{
	union
	{
		struct
		{
			uint32_t edi;
			uint32_t esi;
			uint32_t ebp;
			uint32_t _esp;
			uint32_t ebx;
			uint32_t edx;
			uint32_t ecx;
			uint32_t eax;
			uint32_t _fsgs;
			uint32_t _dses;
			uint32_t eflags;
		} __attribute__((packed));
		struct
		{
			uint16_t di, hdi;
			uint16_t si, hsi;
			uint16_t bp, hbp;
			uint16_t _sp, _hsp;
			uint16_t bx, hbx;
			uint16_t dx, hdx;
			uint16_t cx, hcx;
			uint16_t ax, hax;
			uint16_t gs;
			uint16_t fs;
			uint16_t es;
			uint16_t ds;
			uint16_t flags, hflags;
		} __attribute__((packed));
		struct
		{
			uint8_t dil, dih, edi2, edi3;
			uint8_t sil, sih, esi2, esi3;
			uint8_t bpl, bph, ebp2, ebp3;
			uint8_t _spl, _sph, _esp2, _esp3;
			uint8_t bl, bh, ebx2, ebx3;
			uint8_t dl, dh, edx2, edx3;
			uint8_t cl, ch, ecx2, ecx3;
			uint8_t al, ah, eax2, eax3;
		} __attribute__((packed));
	};
} biosregs_t;

#define EFLAGS_CF	1<<0
#define EFLAGS_PF	1<<2
#define EFLAGS_AF	1<<4
#define EFLAGS_ZF	1<<6
#define EFLAGS_SF	1<<7
#define EFLAGS_TF	1<<8
#define EFLAGS_IF	1<<9
#define EFLAGS_DF	1<<10
#define EFLAGS_OF	1<<11
#define EFLAGS_IOPL	1<<12
#define EFLAGS_NT	1<<14
#define EFLAGS_RF	1<<16
#define EFLAGS_VM	1<<17
#define EFLAGS_AC	1<<18
#define EFLAGS_VIF	1<<19
#define EFLAGS_VIP	1<<20
#define EFLAGS_ID	1<<21

typedef struct
{
	uint64_t address;
	uint64_t length;
	uint32_t type;
} __attribute__((packed)) e820_entry_t;

typedef struct
{
	uint16_t cyl;
	uint8_t head;
	uint16_t resv1;
	uint16_t wpcom;
	uint8_t resv2;
	uint8_t ctl;
	uint8_t resv3;
	uint8_t resv4;
	uint8_t resv5;
	uint16_t lzone;
	uint8_t sect;
	uint8_t resv6;
} __attribute__((packed)) hd_info_t;

typedef struct
{
	uint16_t offset;
	uint16_t segment;
} __attribute__((packed)) fptr16_t;

static inline uint16_t get_segment_from_fptr16(fptr16_t fptr)
{
	return (uint16_t)((*(uint32_t *)&(fptr)) >> 16);
}
static inline uint16_t get_offset_from_fptr16(fptr16_t fptr)
{
	return (uint16_t)((*(uint32_t *)&(fptr)) & 0xffff);
}

static inline fptr16_t get_fptr16_from_logic_address(uint16_t segment, uint16_t offset)
{
	fptr16_t value;
	value.offset = offset;
	value.segment = segment;
	return value;
}

static inline fptr16_t get_fptr16_from_phys_address(uint16_t address)
{
	fptr16_t value;
	value.offset = ((address) & 0xffff);
	value.segment = (((address) >> 16) & 0xffff);
	return value;
}

#define get_value_by_fptr16(fptr, type) \
({ \
	type obj; \
	fptr16_t dest_ptr = get_fptr16_from_logic_address(ss(), (uint32_t)&obj); \
	memcpy_fptr16(dest_ptr, fptr, sizeof(type)); \
	obj; \
})

typedef struct
{
	uint8_t boot_flag;
	uint8_t chs_start[3];
	uint8_t type_id;
	uint8_t chs_end[3];
	uint32_t lba_start;
	uint32_t lba_length;
} __attribute__((packed)) DPT_entry_t;

typedef struct
{
	DPT_entry_t parts[4];
} __attribute__((packed)) DPT_t;

void init_regs(biosregs_t * regs);
void intcall(uint8_t intcall_num, biosregs_t * in_regs, biosregs_t * out_regs);

void console_scroll_lines(int8_t lines);
void console_get_any_key(void);

typedef struct
{
	char vbe_sign[4];
	uint16_t vbe_version;
	uint32_t oem_string_ptr;
	uint8_t capability[4];
	uint32_t video_mode_ptr;
	uint16_t total_memory;
	uint16_t oem_software_revision;
	uint32_t oem_vendor_name_ptr;
	uint32_t oem_product_name_ptr;
	uint32_t oem_product_revision_ptr;
	uint8_t reserved[222];
	uint8_t oem_data[256];
} __attribute__((packed)) vbe_info_block_t;

typedef struct
{
	uint16_t mode_attribute;
	uint8_t window_a_attribute;
	uint8_t window_b_attribute;
	uint16_t window_granularity;
	uint16_t window_size;
	uint16_t window_a_segment;
	uint16_t window_b_segment;
	uint32_t window_function_ptr;
	uint16_t bytes_per_scan_line;
	uint16_t x_resolution;
	uint16_t y_resolution;
	uint8_t x_char_size;
	uint8_t y_char_size;
	uint8_t number_of_planes;
	uint8_t bits_per_pixel;
	uint8_t number_of_banks;
	uint8_t memory_mode;
	uint8_t bank_size;
	uint8_t number_of_image_pages;
	uint8_t reserved1;
	uint8_t red_mask_size;
	uint8_t red_field_position;
	uint8_t green_mask_size;
	uint8_t green_field_position;
	uint8_t blue_mask_size;
	uint8_t blue_field_position;
	uint8_t reserved_mask_size;
	uint8_t reserved_field_position;
	uint8_t direct_color_mode_info;
	uint32_t physical_base_ptr;
	uint32_t reserved2;
	uint16_t reserved3;
	uint16_t linear_bytes_per_scan_line;
	uint8_t bank_number_of_image_pages;
	uint8_t linear_number_of_image_page;
	uint8_t linear_red_mask_size;
	uint8_t linear_red_field_position;
	uint8_t linear_green_mask_size;
	uint8_t linear_green_field_position;
	uint8_t linear_blue_mask_size;
	uint8_t linear_blue_field_position;
	uint8_t linear_reserved_mask_size;
	uint8_t linear_reserved_field_position;
	uint32_t max_pixel_clock;
	uint8_t reserved4[189];
} __attribute__((packed)) vbe_mode_info_block_t;

typedef struct
{
	uint16_t horizontal_total;
	uint16_t horizontal_sync_start;
	uint16_t horizontal_sync_end;
	uint16_t vertical_total;
	uint16_t vertical_sync_start;
	uint16_t vertical_sync_end;
	uint8_t flags;
	uint32_t pixel_clock;
	uint16_t refresh_rate;
	uint8_t reserved[40];
} __attribute__((packed)) vbe_crtc_info_block_t;

void vbe_init(void);
void vbe_set_mode(uint16_t mode_val);
uint16_t vbe_get_current_mode_val(void);
void vbe_get_mode_info(uint16_t mode_val);

size_t strlen_fptr16(fptr16_t fptr);
void memcpy_fptr16(fptr16_t dest_fptr, fptr16_t src_fptr, size_t n);

int printf_fptr16(fptr16_t fptr16);

typedef struct
{
	uint8_t size;
	uint8_t reserved;
	uint16_t count;
	fptr16_t buffer;
	uint64_t start_num;
} __attribute__((packed)) disk_address_packet_t;

bool hd_load_sector_from_disk(uint8_t driver_num, uint32_t lba_start, uint32_t count, fptr16_t buffer);

#endif