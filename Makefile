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
# 磁盘映像分区挂载目录
VDISK	 = /mnt/vdisk

BYTES_PER_SECTOR	= 512
SECTORS_PER_TRACK	= 63
HEADS				= 16
CYLINDER			= 2080
IMAGE_TOTAL_SIZE	= $(shell expr $(BYTES_PER_SECTOR) \* $(SECTORS_PER_TRACK) \* $(HEADS) \* $(CYLINDER))

LOOP_DEVICE_COUNT	= $(shell ls /dev/loop* | grep "^/dev/loop[0-9]\{1,\}$$" | wc -l)

all: Image

# 默认目标，生成磁盘映像文件Image
Image: bootloader os
	if [ ! -f $@ ]; \
	then \
		dd if=/dev/zero of=$@ bs=$(IMAGE_TOTAL_SIZE) count=1; \
	fi
	if [ `wc -c < $@` -ne $(IMAGE_TOTAL_SIZE) ]; \
	then \
		dd if=/dev/zero of=$@ bs=$(IMAGE_TOTAL_SIZE) count=1; \
	fi
	dd if=bootloader/boot of=Image bs=512 count=2 conv=notrunc
	sudo mkdir -p $(VDISK)/p1
	sudo mkdir -p $(VDISK)/p2
	sudo mkdir -p $(VDISK)/p3
	num=0;\
	while [ $$num -ne $(LOOP_DEVICE_COUNT) ]; \
	do \
		sudo losetup -P /dev/loop$$num Image 2>/dev/null; \
		if [ `echo $$?` -ne 0 ]; \
		then \
			num=`expr $$num + 1`; \
			continue; \
		fi; \
		sudo mkfs.fat /dev/loop$${num}p1; \
		sudo mkfs.minix -1 /dev/loop$${num}p3; \
		sudo mount /dev/loop$${num}p1 $(VDISK)/p1; \
		sudo cp bootloader/loader $(VDISK)/p1; \
		sudo cp os/system.elf $(VDISK)/p1; \
		sudo umount $(VDISK)/p1; \
		sudo losetup -d /dev/loop$$num; \
		num=`expr $$num + 1`; \
		break; \
	done; \
	if [ $$num -eq $(LOOP_DEVICE_COUNT) ]; \
	then \
		sudo mknod -m 0660 /dev/loop$(LOOP_DEVICE_COUNT) b 7 $(LOOP_DEVICE_COUNT); \
		sudo losetup -P /dev/loop$(LOOP_DEVICE_COUNT) Image; \
		sudo mkfs.fat /dev/loop$(LOOP_DEVICE_COUNT)p1; \
		sudo mkfs.minix -1 /dev/loop$(LOOP_DEVICE_COUNT)p3; \
		sudo mount /dev/loop$(LOOP_DEVICE_COUNT)p1 $(VDISK)/p1; \
		sudo cp bootloader/loader $(VDISK)/p1; \
		sudo cp os/system.elf $(VDISK)/p1; \
		sudo umount $(VDISK)/p1; \
		sudo losetup -d /dev/loop$(LOOP_DEVICE_COUNT); \
	fi
	@echo "\033[32mCreate Image successfully!\033[0m"

.PHONY: bootloader os tools clean

bootloader:
	$(MAKE) -C bootloader

os:
	$(MAKE) -C os

clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C os clean
	rm -f Image