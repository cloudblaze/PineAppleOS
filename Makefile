#
# /Makefile
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
# Description: 顶层Makefile文件
#

# 项目根目录
export ROOT_DIR		= $(shell pwd)
# Makefile脚本文件目录
export SCRIPTS_DIR	= $(ROOT_DIR)/scripts
# 包含文件搜索目录
export INCLUDE_DIR	= $(ROOT_DIR)/include

BYTES_PER_SECTOR	= 512
SECTORS_PER_TRACK	= 63
HEADS				= 16
CYLINDER			= 2080
IMAGE_TOTAL_SIZE	= $(shell expr $(BYTES_PER_SECTOR) \* $(SECTORS_PER_TRACK) \* $(HEADS) \* $(CYLINDER))

all: Image

# 默认目标，生成磁盘映像文件Image
Image: bootloader os
	if [ ! -f $@ ]; then dd if=/dev/zero of=$@ bs=$(IMAGE_TOTAL_SIZE) count=1; fi
	dd if=bootloader/bootsect of=Image bs=512 conv=notrunc
	sudo losetup -P /dev/loop14 Image
	sudo mkfs.fat /dev/loop14p1
	sudo mount /dev/loop14p1 /mnt/vdisk/p1
	sudo cp bootloader/bootloader /mnt/vdisk/p1
	sudo umount /mnt/vdisk/p1
	sudo losetup -d /dev/loop14

.PHONY: bootloader os tools clean

bootloader:
	$(MAKE) -C bootloader

os:
	$(MAKE) -C os

clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C os clean
	rm -f Image