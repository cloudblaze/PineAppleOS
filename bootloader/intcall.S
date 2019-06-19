/*
 * /bootloader/intcall.S
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
 * Description: 
 *		该程序实现BIOS例程调用包裹，C语言函数声明为：
 *			void intcall(uint8_t intcall_num, biosregs_t * in_regs, biosregs_t * out_regs);
 * 	
 *		在设计时需要诸多寄存器入栈，固规定入展顺序为：
 * 			pushfl
 *			push %ds
 *			push %es
 *			push %fs
 *			push %gs
 *			push %eax
 *			push %edx
 *			push %ecx
 *			push %ebx
 *			push %esp
 *			push %ebp
 *			push %esi
 *			push %edi
 */
	
	.code16
	.section .text
	.global intcall

/******************** function intcall begin ********************/

/*
 * function: 调用BIOS例程。
 * args:
 * 	4(%bp) -- 输入寄存器集
 * 	6(%bp) -- 输出寄存器集
 */
intcall:
					// 56(%sp) -- 输出寄存器集
					// 52(%sp) -- 输入寄存器集
					// 48(%sp) -- BIOS 例程号
					// 44(%sp) -- 返回地址
	pushfl			// 40(%sp)
	push %ds		// 38(%sp)
	push %es		// 36(%sp)
	push %fs		// 34(%sp)
	push %gs		// 32(%sp)
	pushal			// 0(%sp)
	
	mov %sp, %bp
	mov 56(%bp), %ecx
	mov %ecx, out_regs_addr
	mov 48(%bp), %ecx
	mov %cl, intcall_num
	mov 52(%bp), %esi
	sub $44, %sp
	movzx %sp, %edi
	push %ss
	pop %ds
	push %ss
	pop %es
	mov $11, %ecx
	cld
	rep
	movsd

	popal
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popfl

	.byte 0xcd
intcall_num:
	.byte 0

	pushfl
	push %ds
	push %es
	push %fs
	push %gs
	pushal

	cmpw $0, out_regs_addr
	jz 2f

	movzx %sp, %esi
	jmp 1f
out_regs_addr:
	.long 0
1:
	mov out_regs_addr, %edi
	mov $11, %ecx
	cld
	rep
	movsd

2:	add $44, %sp

	popal
	pop %gs
	pop %fs
	pop %ds
	pop %es
	popfl
	retl

/******************** function intcall end ********************/