/*
 * /libc/vsprintf.c
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
 * Description: C标准库程序vsprintf.c
 */

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef BIT16
#include <rm.h>
#endif

#define FLAG_ALTERNATE_FORM 0x01
#define FLAG_ALTERNATE_FORM_CH '#'

#define FLAG_ZERO_PADDING 0x02
#define FLAG_ZERO_PADDING_CH '0'

#define FLAG_LEFT_ADJUST 0x04
#define FLAG_LEFT_ADJUST_CH '-'

#define FLAG_SPACE_BEFORE_POSITIVE_NUM 0x08
#define FLAG_SPACE_BEFORE_POSITIVE_NUM_CH ' '

#define FLAG_SIGN 0x10
#define FLAG_SIGN_CH '+'

#define FLAG_LOWER 0x20

#define INT_TYPE_CHAR 0x1
#define INT_TYPE_SHORT 0x2
#define INT_TYPE_INT 0x4
#define INT_TYPE_LONG 0x8
#define INT_TYPE_LONG_LONG 0x10
#define INT_TYPE_MIN INT_TYPE_CHAR
#define INT_TYPE_MAX INT_TYPE_LONG_LONG

#define BUFFER_SIZE 4096

static char number_string_buffer[BUFFER_SIZE];

/*
 * function: 将32位有符号整数转换为字符串
 * args:
 * 	@number -- 32位有符号整数
 * 	@flag -- 格式标志
 *  @width -- 宽度
 * return:
 * 	转换后的字符串
 */
static char * int32_to_string_decimal(int32_t number, int flag, int width)
{
	int32_t num_tmp = number;
	char *p = number_string_buffer;
	char *q = NULL;
	int length = 0;
	char *ptr_first = NULL;
	char *ptr_end = NULL;
	char ch = 0;

	memset(number_string_buffer, 0, sizeof(number_string_buffer));

	char dic[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	if (num_tmp < 0)
	{
		*p++ = '-';
	}
	else if (flag & FLAG_SIGN)
	{
		*p++ = '+';
	}
	else
	{
		*p++ = ' ';
	}
	ptr_first = number_string_buffer;

	if (num_tmp >= 0 && !(flag & FLAG_SIGN))
	{
		ptr_first++;
	}

	if (num_tmp == 0)
	{
		*p++ = '0';
	}
	else
	{
		if (num_tmp < 0)
			num_tmp = -num_tmp;

		while (num_tmp)
		{
			*p++ = dic[num_tmp % 10];
			num_tmp /= 10;
		}
	}
	*p = '\0';

	ptr_end = p;
	length = ptr_end - ptr_first;

	q = number_string_buffer + 1;
	p--;
	while (q < p)
	{
		ch = *q;
		*q = *p;
		*p = ch;
		p--;
		q++;
	}

	if (length < width)
	{
		p = ptr_end;

		if(flag & FLAG_LEFT_ADJUST)
		{
			for(int i = 0; i < width - length; i++)
				*p++ = ' ';
			*p = '\0';
			ptr_end = p;
		}
		else
		{
			while (p >= ptr_first)
			{
				*(p + width - length) = *p;
				p--;
			}

			if (flag & FLAG_ZERO_PADDING)
			{
				for (int i = 0; i < width - length; i++)
				{
					number_string_buffer[i + 1] = '0';
				}
			}
			else
			{
				for (int i = 0; i < width - length; i++)
					ptr_first[i] = ' ';
			}
		}
	}

	return ptr_first;
}

static char *int64_to_string_decimal(int64_t number, int flag, int width)
{
	int64_t num_tmp = number;
	char *p = number_string_buffer;
	char *q = NULL;
	int length = 0;
	char *ptr_first = NULL;
	char *ptr_end = NULL;
	char ch = 0;

	memset(number_string_buffer, 0, sizeof(number_string_buffer));

	char dic[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	if (num_tmp < 0)
	{
		*p++ = '-';
	}
	else if (flag & FLAG_SIGN)
	{
		*p++ = '+';
	}
	else
	{
		*p++ = ' ';
	}
	ptr_first = number_string_buffer;

	if (num_tmp >= 0 && !(flag & FLAG_SIGN))
	{
		ptr_first++;
	}

	if (num_tmp == 0)
	{
		*p++ = '0';
	}
	else
	{
		if (num_tmp < 0)
			num_tmp = -num_tmp;

		while (num_tmp)
		{
			uint32_t remainder = 0;
			num_tmp = div_u64(num_tmp, 10, &remainder);
			*p++ = dic[remainder];
		}
	}
	*p = '\0';

	ptr_end = p;
	length = ptr_end - ptr_first;

	q = number_string_buffer + 1;
	p--;
	while (q < p)
	{
		ch = *q;
		*q = *p;
		*p = ch;
		p--;
		q++;
	}

	if (length < width)
	{
		p = ptr_end;

		if(flag & FLAG_LEFT_ADJUST)
		{
			for(int i = 0; i < width - length; i++)
				*p++ = ' ';
			*p = '\0';
			ptr_end = p;
		}
		else
		{
			while (p >= ptr_first)
			{
				*(p + width - length) = *p;
				p--;
			}

			if (flag & FLAG_ZERO_PADDING)
			{
				for (int i = 0; i < width - length; i++)
				{
					number_string_buffer[i + 1] = '0';
				}
			}
			else
			{
				for (int i = 0; i < width - length; i++)
					ptr_first[i] = ' ';
			}
		}
	}

	return ptr_first;
}

static char *uint32_to_string_hexadecimal(uint32_t number, int flag, int width)
{
	uint32_t num_tmp = number;
	char *p = number_string_buffer;
	char *q = NULL;
	int length = 0;
	char *ptr_first = NULL;
	char *ptr_end = NULL;
	char ch = 0;

	memset(number_string_buffer, 0, sizeof(number_string_buffer));

	char dic_lower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	char dic_upper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	char *dic = (flag & FLAG_LOWER) ? dic_lower : dic_upper;

	ptr_first = number_string_buffer;

	*p++ = '0';
	*p++ = (flag & FLAG_LOWER) ? 'x' : 'X';

	if (!(flag & FLAG_ALTERNATE_FORM))
	{
		ptr_first += 2;
	}

	do
	{
		*p++ = dic[num_tmp % 16];
		num_tmp /= 16;
	} while (num_tmp);
	*p = '\0';

	ptr_end = p;
	length = ptr_end - ptr_first;

	q = number_string_buffer + 2;
	p--;
	while (q < p)
	{
		ch = *q;
		*q = *p;
		*p = ch;
		p--;
		q++;
	}

	if (length < width)
	{
		p = ptr_end;

		if(flag & FLAG_LEFT_ADJUST)
		{
			for(int i = 0; i < width - length; i++)
				*p++ = ' ';
			*p = '\0';
			ptr_end = p;
		}
		else
		{
			while (p >= ptr_first)
			{
				*(p + width - length) = *p;
				p--;
			}
			if(flag & FLAG_ALTERNATE_FORM)
				ptr_first[1] = (flag & FLAG_LOWER) ? 'x' : 'X';

			if (flag & FLAG_ZERO_PADDING)
			{
				for (int i = 0; i < width - length; i++)
				{
					number_string_buffer[i + 2] = '0';
				}
			}
			else
			{
				for (int i = 0; i < width - length; i++)
					ptr_first[i] = ' ';
			}
		}
	}

	if(number == 0 && flag & FLAG_ALTERNATE_FORM)
		ptr_first[1] = '0';

	return ptr_first;
}

static char *uint64_to_string_hexadecimal(uint64_t number, int flag, int width)
{
	uint64_t num_tmp = number;
	char *p = number_string_buffer;
	char *q = NULL;
	int length = 0;
	char *ptr_first = NULL;
	char *ptr_end = NULL;
	char ch = 0;

	memset(number_string_buffer, 0, sizeof(number_string_buffer));

	char dic_lower[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	char dic_upper[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	char *dic = (flag & FLAG_LOWER) ? dic_lower : dic_upper;

	ptr_first = number_string_buffer;

	*p++ = '0';
	*p++ = (flag & FLAG_LOWER) ? 'x' : 'X';

	if (!(flag & FLAG_ALTERNATE_FORM))
	{
		ptr_first += 2;
	}

	while (num_tmp)
	{
		*p++ = dic[num_tmp % 16];
		num_tmp /= 16;
	}
	*p = '\0';

	ptr_end = p;
	length = ptr_end - ptr_first;

	q = number_string_buffer + 2;
	p--;
	while (q < p)
	{
		ch = *q;
		*q = *p;
		*p = ch;
		p--;
		q++;
	}

	if (length < width)
	{
		p = ptr_end;

		while (p >= ptr_first)
		{
			*(p + width - length) = *p;
			p--;
		}

		if (flag & FLAG_ZERO_PADDING)
		{
			for (int i = 0; i < width - length; i++)
			{
				number_string_buffer[i + 2] = '0';
			}
		}
		else
		{
			for (int i = 0; i < width - length; i++)
				ptr_first[i] = ' ';
		}
	}

	return ptr_first;
}

static char *uint32_to_string_octal(uint32_t number, int flag, int width)
{
	uint32_t num_tmp = number;
	char *p = number_string_buffer;
	char *q = NULL;
	int length = 0;
	char *ptr_first = NULL;
	char *ptr_end = NULL;
	char ch = 0;

	memset(number_string_buffer, 0, sizeof(number_string_buffer));

	char dic[] = {'0', '1', '2', '3', '4', '5', '6', '7'};

	ptr_first = number_string_buffer;

	*p++ = '0';

	if (!(flag & FLAG_ALTERNATE_FORM))
	{
		ptr_first += 1;
	}

	while (num_tmp)
	{
		*p++ = dic[num_tmp % 8];
		num_tmp /= 8;
	}
	*p = '\0';

	ptr_end = p;
	length = ptr_end - ptr_first;

	q = number_string_buffer + 1;
	p--;
	while (q < p)
	{
		ch = *q;
		*q = *p;
		*p = ch;
		p--;
		q++;
	}

	if (length < width)
	{
		p = ptr_end;

		if(flag & FLAG_LEFT_ADJUST)
		{
			for(int i = 0; i < width - length; i++)
				*p++ = ' ';
			*p = '\0';
			ptr_end = p;
		}
		else
		{
			while (p >= ptr_first)
			{
				*(p + width - length) = *p;
				p--;
			}

			if (flag & FLAG_ZERO_PADDING)
			{
				for (int i = 0; i < width - length; i++)
				{
					number_string_buffer[i + 1] = '0';
				}
			}
			else
			{
				for (int i = 0; i < width - length; i++)
					ptr_first[i] = ' ';
			}
		}
	}

	return ptr_first;
}

/*
 * function: 在buffer缓冲区中追加字符串。
 * args:
 * 	@buffer -- 缓冲区
 *  @string -- 待追加的字符串
 * return:
 * 	缓冲区中追加的字符串后面的位置
 */
static char * insert_string(char *buffer, const char *string)
{
	char *p = buffer;

	while (*string)
	{
		*p++ = *string++;
	}

	return p;
}

/*
 * function: 格式化输出字符串
 * args:
 * 	@buffer: 字符串缓冲区
 * 	@format: 格式化字符串
 *  @args: 可变参数
 * return:
 * 	如果成功，则返回输出的字符数；如果失败则返回一个负数。
 */
int vsprintf(char *buffer, const char *format, va_list args)
{
	char *string = buffer;
	int flag = 0;
	int int_type = INT_TYPE_INT;
	int tot_width = 0;
	int sub_width = 0;

	char *s = NULL;
	char ch = 0;
	int8_t num_8 = 0;
	uint8_t num_u8 = 0;
	int16_t num_16 = 0;
	uint16_t num_u16 = 0;
	int32_t num_32 = 0;
	uint32_t num_u32 = 0;
	int64_t num_64 = 0;
	uint64_t num_u64 = 0;

	for (const char *p = format; *p; p++)
	{
		if (*p != '%')
		{
			*string++ = *p;
			if(*p == '\n')
				*string++ = '\r';
			continue;
		}

		flag = 0;
		tot_width = 0;
		sub_width = 0;
		int_type = INT_TYPE_INT;

		p++;

		while (*p == FLAG_ALTERNATE_FORM_CH || *p == FLAG_ZERO_PADDING_CH || *p == FLAG_LEFT_ADJUST_CH || *p == FLAG_SPACE_BEFORE_POSITIVE_NUM_CH || *p == FLAG_SIGN_CH)
		{
			if (*p == FLAG_ALTERNATE_FORM_CH)
			{
				flag |= FLAG_ALTERNATE_FORM;
			}
			else if (*p == FLAG_ZERO_PADDING_CH)
			{
				flag |= FLAG_ZERO_PADDING;
			}
			else if (*p == FLAG_LEFT_ADJUST_CH)
			{
				flag |= FLAG_LEFT_ADJUST;
				flag &= ~FLAG_ZERO_PADDING;
			}
			else if (*p == FLAG_SPACE_BEFORE_POSITIVE_NUM_CH)
			{
				flag |= FLAG_SPACE_BEFORE_POSITIVE_NUM;
			}
			else if (*p == FLAG_SIGN_CH)
			{
				flag |= FLAG_SIGN;
			}
			else
			{
			}

			p++;
		}

		if(*p == '*')
		{
			tot_width = va_arg(args, int);
			if(tot_width < 0)
				tot_width = 0;
			p++;
		}
		else
		{
			while (isdigit(*p))
			{
				tot_width = tot_width * 10 + *p - '0';
				p++;
			}
		}
		if (*p == '.')
		{
			if(*p == '*')
			{
				sub_width = va_arg(args, int);
				if(sub_width < 0)
					sub_width = 0;
				p++;
			}
			else
			{
				while (isdigit(*p))
				{
					sub_width = sub_width * 10 + *p - '0';
					p++;
				}
			}
		}

LOOP_switch:
		switch (*p)
		{
		case 'h':
			p++;
			if(int_type >= INT_TYPE_MIN)
			{
				int_type >>= 1;
				goto LOOP_switch;
			}
			else
			{
				*string++ = '%';
				break;
			}
		case 'l':
			p++;
			if(int_type <= INT_TYPE_MAX)
			{
				int_type <<= 1;
				goto LOOP_switch;
			}
			else
			{
				*string++ = '%';
				break;
			}
		case 's':
			s = va_arg(args, char *);
			string = insert_string(string, s);
			break;
		case 'c':
			ch = (char)(va_arg(args, int) & 0xFF);
			*string++ = ch;
			break;
		case 'd':
			switch(int_type)
			{
				case INT_TYPE_CHAR:
					num_8 = (int8_t)va_arg(args, int32_t);
					string = insert_string(string, int32_to_string_decimal(num_8, flag, tot_width));
					break;
				case INT_TYPE_SHORT:
					num_16 = (int16_t)va_arg(args, int32_t);
					string = insert_string(string, int32_to_string_decimal(num_16, flag, tot_width));
					break;
				case INT_TYPE_INT:
					num_32 = va_arg(args, int32_t);
					string = insert_string(string, int32_to_string_decimal(num_32, flag, tot_width));
					break;
				case INT_TYPE_LONG:
					num_32 = va_arg(args, int32_t);
					string = insert_string(string, int64_to_string_decimal(num_64, flag, tot_width));
					break;
				case INT_TYPE_LONG_LONG:
					num_64 = va_arg(args, int64_t);
					string = insert_string(string, int64_to_string_decimal(num_64, flag, tot_width));
					break;
			}
			break;
		case 'x':
			flag |= FLAG_LOWER;
		case 'X':
			switch(int_type)
			{
				case INT_TYPE_CHAR:
					num_u8 = (uint8_t)va_arg(args, uint32_t);
					string = insert_string(string, uint32_to_string_hexadecimal(num_u8, flag, tot_width));
					break;
				case INT_TYPE_SHORT:
					num_u16 = (uint16_t)va_arg(args, uint32_t);
					string = insert_string(string, uint32_to_string_hexadecimal(num_u16, flag, tot_width));
					break;
				case INT_TYPE_INT:
					num_u32 = va_arg(args, uint32_t);
					string = insert_string(string, uint32_to_string_hexadecimal(num_u32, flag, tot_width));
					break;
				case INT_TYPE_LONG:
					num_u32 = va_arg(args, uint32_t);
					string = insert_string(string, uint64_to_string_hexadecimal(num_u64, flag, tot_width));
					break;
				case INT_TYPE_LONG_LONG:
					num_u64 = va_arg(args, uint64_t);
					string = insert_string(string, uint64_to_string_hexadecimal(num_u64, flag, tot_width));
					break;
			}
			break;
		case 'o':
			num_u32 = va_arg(args, uint32_t);
			string = insert_string(string, uint32_to_string_octal(num_u32, flag, tot_width));
			break;
		case '%':
			*string++ = '%';
			break;
		default:
			*string++ = '%';
			*string++ = *p;
			break;
		}
	}
	*string = '\0';

	return string - buffer;
}