#include <system.h>

extern unsigned long vectors;

int interrupts_init(void)
{
	int el = current_el();

	switch (el) {
	case 3:
		asm volatile("msr vbar_el3, %0" : : "r" (&vectors) : "cc");
		/* Fall through */
	case 2:
		asm volatile("msr vbar_el2, %0" : : "r" (&vectors) : "cc");
		/* Fall through */
	case 1:
		asm volatile("msr vbar_el1, %0" : : "r" (&vectors) : "cc");
		/* Fall through */
	default:
		break;
	
	
	}

	return 0;
}
