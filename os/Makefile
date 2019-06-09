#
# /os/Makefile
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
# Created by Huo Yun - 2019-06-07
# Description: os目录子Makefile文件
#

# 包含Makefile变量定义文件及规则定义文件
include $(SCRIPTS_DIR)/Makefile.variable
include $(SCRIPTS_DIR)/Makefile.rule

# 二进制文件
BIN_FILE	= system system.elf
# 目标文件
OBJ_FILE	= $(SYSTEM_OBJ)
# 中间过程临时文件
TMP_FILE	:=
# 连接图文件
MAP_FILE	:=

all: $(BIN_FILE)

ASFLAGS = $(ASFLAGS_32)
CFLAGS	= $(CFLAGS_32) -Iinclude
LDFLAGS = $(LDFLAGS_32) -Ttext=0x100000

# 注释为最终的代码，但需要逐步编码
# SYSTEM_OBJ	= system.o \
# 				init/main.o \
# 				kernel/kprintf.o \
# 				task/sched.o task/pcb_list.o task/fork.o \
# 				intr/idt.o intr/interrupt.o intr/intr_handler_entry.o intr/exception.o \
# 				syscall/system_trap_handler.o syscall/system_trap.o syscall/sys.o\
# 				mm/gdt.o mm/paging.o mm/mm.o mm/main_mem_page.o mm/dynamic_mem.o \
# 				drivers/chr_dev/console.o drivers/chr_dev/vga.o \
# 				drivers/chr_dev/keyboard.o \
# 				datastruct/list.o
SYSTEM_OBJ		= system.o

system: $(SYSTEM_OBJ)

system.elf: $(SYSTEM_OBJ)
MAP_FILE	+= system.map

system.o: system.s
TMP_FILE	+= system.s

# 注释为最终的代码，但需要逐步编码
# system.s: system.S
# drivers/chr_dev/keyboard.s: drivers/chr_dev/keyboard.S
# intr/intr_handler_entry.s: intr/intr_handler_entry.S
# syscall/system_trap_handler.s: syscall/system_trap_handler.S

.PHONY: clean

clean:
	rm -f $(TMP_FILE) \
		$(OBJ_FILE) \
		$(MAP_FILE) \
		$(BIN_FILE)