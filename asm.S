#include <assembler64.h>

.global __vcpu_save_context
__vcpu_save_context:
	sub  sp, sp, #32
	push x28, x29
	push x26, x27
	push x24, x25
	push x22, x23
	push x20, x21
	push x18, x19
	push x16, x17
	push x14, x15
	push x12, x13
	push x10, x11
	push x8, x9
	push x6, x7
	push x4, x5
	push x2, x3
	push x0, x1
	mrs  x0, sp_el0
	mrs  x1, sp_el1
	mrs  x2, spsr_el2
	mrs  x3, elr_el2
	stp  x3, x0, [sp, #0xF8]
	stp  x1, x2, [sp, #0xF8 + 16]
	ret

.global __vcpu_restore_context
__vcpu_restore_context:
	ldp  x3, x0, [sp, #0xF8]
	ldp  x1, x2, [sp, #0xF8 + 16]
	msr  sp_el0, x0
	msr  sp_el1, x1
	msr  spsr_el2, x2
	msr  elr_el2, x3
	pop x0, x1
	pop x2, x3
	pop x4, x5
	pop x6, x7
	pop x8, x9
	pop x10, x11
	pop x12, x13
	pop x14, x15
	pop x16, x17
	pop x18, x19
	pop x20, x21
	pop x22, x23
	pop x24, x25
	pop x26, x27
	pop x28, x29
	add sp, sp, #32
	ret	

.global __vm_start
__vm_start:
	mov sp, x0
	bl __vcpu_restore_context
	eret
#	mrs lr, elr_el2
#	ret
