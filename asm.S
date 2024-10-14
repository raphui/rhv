#define lr x30
#define regsave_long_offset 0xf0
#define regsave_short_offset 0x90

.macro push ra, rb
	stp \ra, \rb, [sp,#-16]!
.endm

.macro pop ra, rb
	ldp \ra, \rb, [sp], #16
.endm

.macro regsave_long
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
	mrs  x1, elr_el1
	mrs  x2, spsr_el1
	stp  lr, x0, [sp, #regsave_long_offset]
	stp  x1, x2, [sp, #regsave_long_offset + 16]
.endm

.macro regsave_short
	sub  sp, sp, #32
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
	mrs  x1, elr_el1
	mrs  x2, spsr_el1
	stp  lr, x0, [sp, #regsave_short_offset]
	stp  x1, x2, [sp, #regsave_short_offset + 16]
.endm

.macro regrestore_long
	ldp  lr, x0, [sp, #regsave_long_offset]
	ldp  x1, x2, [sp, #regsave_long_offset + 16]
	msr  sp_el0, x0
	msr  elr_el1, x1
	msr  spsr_el1, x2
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
.endm

.macro regrestore_short
	ldp  lr, x0, [sp, #regsave_short_offset]
	ldp  x1, x2, [sp, #regsave_short_offset + 16]
	msr  sp_el0, x0
	msr  elr_el1, x1
	msr  spsr_el1, x2
	pop x0, x1
	pop x2, x3
	pop x4, x5
	pop x6, x7
	pop x8, x9
	pop x10, x11
	pop x12, x13
	pop x14, x15
	pop x16, x17
	add sp, sp, #32
.endm




.global __save_vcpu_context
__save_vcpu_context:
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
	stp  lr, x0, [sp, #regsave_long_offset]
	stp  x1, x2, [sp, #regsave_long_offset + 16]

.global __restore_vcpu_context
__restore_vcpu_context:
	ldp  lr, x0, [sp, #regsave_long_offset]
	ldp  x1, x2, [sp, #regsave_long_offset + 16]
	msr  sp_el0, x0
	msr  sp_el1, x1
	msr  spsr_el2, x2
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
