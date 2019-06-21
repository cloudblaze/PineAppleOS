/*
 * /bootloader/stdio.h
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
 * Description: C标准库头文件stdio.h
 */

#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

/*
 * function: 格式化输出字符串
 * args:
 * 	@buffer: 字符串缓冲区
 * 	@format: 格式化字符串
 *  @args: 可变参数
 * return:
 * 	如果成功，则返回输出的字符数；如果失败则返回一个负数。
 */
int vsprintf(char *buffer, const char *format, va_list args);

/*
 * function: 格式化输出字符串
 * args:
 * 	@format -- 格式化字符串
 * return:
 * 	如果成功，则返回输出的字符数；如果失败则返回一个负数。
 */
int printf(const char * format, ...);

#endif