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

#ifdef BIT16
__asm__(".code16gcc");
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef BIT16
void * memset16(void * s, int c, size_t n)
#else
void * memset(void * s, int c, size_t n)
#endif
{
    uint8_t * p = s;

    for(int i = 0; i < n; i++)
    {
        *(p + i) = (uint8_t)c;
    }

    return s;
}

#ifdef BIT16
size_t strlen16(const char * str)
#else
size_t strlen(const char * str)
#endif
{
    size_t length = 0;

    while(*str++)
        length++;
    
    return length;
}