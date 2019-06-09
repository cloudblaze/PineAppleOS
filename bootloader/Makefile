#
# /bootloader/Makefile
# This file is part of PineAppleOS
#
# Copyright (C) 2019 - Huo Yun
#
# PineAppleOS is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# PineAppleOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PineAppleOS. If not, see <http://www.gnu.org/licenses/>.
#

#
# Created by Huo Yun - 2019-06-06
# Description: bootloader目录子Makefile文件
#

# 包含Makefile变量定义文件及规则定义文件
include $(SCRIPTS_DIR)/Makefile.variable
include $(SCRIPTS_DIR)/Makefile.rule

sources_S = $(shell ls *.S)

# 二进制文件
BINARY_FILE	= bootsect bootloader
# 目标文件
OBJECT_FILE	= $(BOOTSECT_OBJ) $(BOOTLOADER_OBJ)
# 中间过程临时文件
TEMPORARY_FILE	:= $(sources_S:.S=.s)
# 连接图文件
MAP_FILE		:=
# 依赖文件
DEPENDENT_FILE	:= $(sources_S:.S=.d)

all: $(BINARY_FILE)

ASFLAGS = $(ASFLAGS_16)
CFLAGS	= $(CFLAGS_16) -I.
LDFLAGS = $(LDFLAGS_16) -Ttext=0x0

BOOTSECT_OBJ = bootsect.o

bootsect: $(BOOTSECT_OBJ)
MAP_FILE	+= bootsect.map

# 注释为最终的代码，但需要逐步编码
# BOOTLOADER_OBJ	= system.o main16.o \
# 					rm.o intcall.o \
# 					console.o vbe.o printf.o vsprintf16.o string.o \
# 					main32.o
BOOTLOADER_OBJ	= bootloader.o

bootloader: $(BOOTLOADER_OBJ)
MAP_FILE	+= bootloader.map

# 注释为最终的代码，但需要逐步编码
#main16.o: main16.c

include $(sources_S:.S=.d)

.PHONY: clean

clean:
	rm -f $(TEMPORARY_FILE) \
		$(OBJECT_FILE) \
		$(MAP_FILE) \
		$(DEPENDENT_FILE) \
		$(BINARY_FILE)