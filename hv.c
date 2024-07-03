#include <hv.h>
#include <interrupts.h>
#include <printf.h>
#include <pl011.h>

void hv(void)
{
	interrupts_init();

	pl011_init();

	printf("Hello World!\n");

	while(1)
		;
}
