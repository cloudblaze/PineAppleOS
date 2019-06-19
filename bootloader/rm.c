/*
 * /bootloader/rm.c
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
 * Description: 与实模式有关的函数
 */

#include <string.h>

#include "rm.h"

void init_regs(biosregs_t * regs)
{
    memset(regs, 0, sizeof(biosregs_t));
    regs->eflags |= EFLAGS_CF;
    regs->ds = ds();
    regs->es = es();
    regs->fs = fs();
    regs->gs = gs();
}