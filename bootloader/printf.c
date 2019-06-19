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

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define _console_print console_print
void _console_print(const char * str);

#define BUFFER_SIZE 4096
static char string_buffer[BUFFER_SIZE];

int vsprintf(char *buf, const char *format, va_list args);

int sprintf(char * buf, const char *format, ...)
{
	int char_count = 0;
	va_list args;

	va_start(args, format);
	char_count = vsprintf(buf, format, args);
	va_end(args);

	return char_count;
}

int printf(const char *format, ...)
{
	int char_count = 0;
	va_list args;

	va_start(args, format);
	char_count = vsprintf(string_buffer, format, args);
	va_end(args);
	_console_print(string_buffer);

	return char_count;
}

#ifdef BIT16
#include <rm.h>

#define _console_print_fptr16 console_print_fptr16
size_t _console_print_fptr16(fptr16_t fptr);

int printf_fptr16(fptr16_t fptr)
{
	return _console_print_fptr16(fptr);
}
#endif