#include <hv.h>
#include <interrupts.h>

void hv(void)
{
	interrupts_init();

	while(1)
		;
}
