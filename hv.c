#include <hv.h>
#include <interrupts.h>
#include <printf.h>
#include <pl011.h>
#include <mmu.h>
#include <mm/mm.h>
#include <sizes.h>
#include <vm.h>

extern uint64_t _stext[];
extern uint64_t _etext[];

void hv(void)
{
	struct vm *vm = NULL;

	interrupts_init();

	pl011_init();

	heap_init();

	vm = vm_create(1, 0xABCDABCD);
	if (!vm) {
		printf("fail to create vm\n");
	}

#if 0
	mmu_enable();

	mmu_remap_range(0x0, 0x0, PAGE_SIZE, MAP_FAULT);
#endif

//#define MMU_TEST
#ifdef MMU_TEST
	printf("Testing MMU...\n");
	printf("Access to 0x%x...", &_stext);

	uint64_t val = *(uint64_t *)_stext;

	printf("OK\n");

	printf("Map 0x%x as fault\n", &_stext);

	mmu_remap_range(&_stext, &_stext, PAGE_SIZE, MAP_FAULT);
	printf("Access to 0x%x...", &_stext);

	val = *(uint64_t *)_stext;

	printf("OK\n");
#endif

	while(1)
		;
}
