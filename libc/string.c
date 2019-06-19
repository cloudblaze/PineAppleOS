/*
 * /libc/string.c
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
 * Description: C标准库程序string.c
 */

#include <stddef.h>
#include <stdint.h>

void * memset(void * s, int c, size_t n)
{
    uint8_t * p = s;

    for(int i = 0; i < n; i++)
    {
        *(p + i) = (uint8_t)c;
    }

    return s;
}

size_t strlen(const char * str)
{
    size_t length = 0;

    while(*str++)
        length++;
    
    return length;
}

#ifdef BIT16
#include <rm.h>

size_t strlen_fptr16(fptr16_t fptr)
{	
	size_t length = 0;

	__asm__(
		"push %%ds\n\t"
		"mov %2, %%ds\n\t"
		"1:lodsb\n\t"
		"test %%al, %%al\n\t"
		"jz 2f\n\t"
		"inc %0\n\t"
		"jmp 1b\n\t"
		"2:pop %%ds"
		: "=r"(length)
		: "0"(0), "r"(get_segment_from_fptr16(fptr)), "S"(get_offset_from_fptr16(fptr))
		: "eax");
	
	return length;
}

void memcpy_fptr16(fptr16_t dest_fptr, fptr16_t src_fptr, size_t n)
{
	__asm__(
		"push %%ds\n\t"
		"push %%es\n\t"
		"mov %1, %%ds\n\t"
		"mov %3, %%es\n\t"
		"cld;rep;movsb;\n\t"
		"pop %%es\n\t"
		"pop %%ds"
		:: "c"(n), "r"(get_segment_from_fptr16(src_fptr)), "S"(get_offset_from_fptr16(src_fptr)), "r"(get_segment_from_fptr16(dest_fptr)), "D"(get_offset_from_fptr16(dest_fptr)));
}
#endif