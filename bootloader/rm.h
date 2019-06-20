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

#include <stdint.h>

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

/*
此处的注释需要保留，因为不确定预处理方式和静态内联方式中哪种方式有效。
#define div_u64(num_u64, num_u32, remainder) \
({ \
	uint64_t num1 = (num_u64); \
	uint32_t num2 = (num_u32); \
	uint32_t rslt = 0; \
	__asm__( \
		"div %4" \
		: "=a"(rslt), "=d"(remainder) \
		:"0"(*(int32_t *)&num1), "1"(*((int32_t *)&num1 + 1)), "b"(num2)); \
	rslt; \
})
*/

static inline uint32_t div_u64(uint64_t num1, uint32_t num2, uint32_t * remainder)
{
	uint32_t result = 0;

	__asm__(
		"div %4"
		: "=a"(result), "=d"(*remainder)
		:"0"(*(uint32_t *)&num1), "1"(*((uint32_t *)&num1 + 1)), "b"(num2));
	
	return result;
}

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


#endif