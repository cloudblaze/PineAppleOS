/*
 * /bootloader/printf.c
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
 * Description: C标准库程序printf.c
 */

#ifdef BIT16
__asm__(".code16gcc");
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void _console_print(const char * str);
#ifdef BIT16
#define _console_print console16_print
#endif

#define BUFFER_SIZE 4096
static char string_buffer[BUFFER_SIZE];

int vsprintf(char *buf, const char *format, va_list args);
#ifdef BIT16
#define vsprintf vsprintf16
#endif

#ifdef BIT16
int sprintf16(char * buf, const char *format, ...)
#else
int sprintf(char * buf, const char *format, ...)
#endif
{
	int char_count = 0;
	va_list args;

	va_start(args, format);
	char_count = vsprintf(buf, format, args);
	va_end(args);

	return char_count;
}

/*
 * function: 格式化输出字符串
 * args:
 * 	@format -- 格式化字符串
 * return:
 * 	如果成功，则返回输出的字符数；如果失败则返回一个负数。
 */
#ifdef BIT16
int printf16(const char *format, ...)
#else
int printf(const char *format, ...)
#endif
{
	int char_count = 0;
	va_list args;

	va_start(args, format);
	char_count = vsprintf(string_buffer, format, args);
	va_end(args);
	_console_print(string_buffer);

	return char_count;
}