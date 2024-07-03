#include <assembler64.h>

.global _start
_start:
	switch_el x0, entry_el3, entry_el2, entry_el1

entry_el3:
	adr x0, entry_el2
	msr ELR_EL3, x0

	/* Mask all interrupts */
	mov x0, #0x9
	/* Set the target EL and stack */
	mov x1, #0x3C0
	orr x0, x1, x0
	msr SPSR_EL3, x0

	adr x0, _stack
	msr SP_EL2, x0

	eret

entry_el2:
	adr x1, vectors
	msr VBAR_EL2, x1

	mrs x0, SPSR_EL2
	/* Mask all interrupts */
	mov x0, #0x9
	/* Set the target EL and stack */
	mov x1, #0x3C0
	orr x0, x1, x0
	msr SPSR_EL2, x0

	adr x0, _stack
	mov sp, x0

	b hv

entry_el1:
	// Nothing to do, too late
	b entry_el1
