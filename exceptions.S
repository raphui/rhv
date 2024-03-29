#include <assembler64.h>

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro	exception_entry
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	switch_el x11, 3f, 2f, 1f
3:	mrs	x1, esr_el3
	mrs	x2, elr_el3
	mrs     x3, far_el3
	b	0f
2:	mrs	x1, esr_el2
	mrs	x2, elr_el2
	mrs     x3, far_el2
	b	0f
1:	mrs	x1, esr_el1
	mrs	x2, elr_el1
	mrs     x3, far_el1
0:
	stp	x2, x0, [sp, #-16]!
	mov	x0, sp
.endm

/*
 * Exception vectors.
 */
	.align	11
	.globl	vectors
vectors:
	.align	7
	b	_do_bad_sync	/* Current EL Synchronous Thread */

	.align	7
	b	_do_bad_irq	/* Current EL IRQ Thread */

	.align	7
	b	_do_bad_fiq	/* Current EL FIQ Thread */

	.align	7
	b	_do_bad_error	/* Current EL Error Thread */

	.align	7
	b	_do_sync	/* Current EL Synchronous Handler */

	.align	7
	b	_do_irq		/* Current EL IRQ Handler */

	.align	7
	b	_do_fiq		/* Current EL FIQ Handler */

	.align	7
	b	_do_error	/* Current EL Error Handler */


_do_bad_sync:
	exception_entry
	bl	_do_bad_sync
	b	exception_exit

_do_bad_irq:
	exception_entry
	bl	_do_bad_irq
	b	exception_exit

_do_bad_fiq:
	exception_entry
	bl	_do_bad_fiq
	b	exception_exit

_do_bad_error:
	exception_entry
	bl	_do_bad_error
	b	exception_exit

_do_sync:
	exception_entry
	mov	x2, x3
	bl	_do_sync
	b	exception_exit

_do_irq:
	exception_entry
	bl	_do_irq
	b	exception_exit

_do_fiq:
	exception_entry
	bl	_do_fiq
	b	exception_exit

_do_error:
	exception_entry
	bl	_do_error
	b	exception_exit

exception_exit:
	ldp	x2, x0, [sp],#16
	switch_el x11, 3f, 2f, 1f
3:     	msr	elr_el3, x2
	b	0f
2:     	msr	elr_el2, x2
	b	0f
1:     	msr	elr_el1, x2
0:
	ldp	x1, x2, [sp],#16
	ldp	x3, x4, [sp],#16
	ldp	x5, x6, [sp],#16
	ldp	x7, x8, [sp],#16
	ldp	x9, x10, [sp],#16
	ldp	x11, x12, [sp],#16
	ldp	x13, x14, [sp],#16
	ldp	x15, x16, [sp],#16
	ldp	x17, x18, [sp],#16
	ldp	x19, x20, [sp],#16
	ldp	x21, x22, [sp],#16
	ldp	x23, x24, [sp],#16
	ldp	x25, x26, [sp],#16
	ldp	x27, x28, [sp],#16
	ldp	x29, x30, [sp],#16
	eret

.section .data
.align 4
.global arm_ignore_data_abort
arm_ignore_data_abort:
.word 0 /* When != 0 data aborts are ignored */
.global arm_data_abort_occurred
arm_data_abort_occurred:
.word 0 /* set != 0 by the data abort handler */
abort_stack:
.space 8
