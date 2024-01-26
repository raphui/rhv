#include <init.h>
#include <mm/memory.h>

int heap_size = MAX_KERNEL_HEAP_SIZE;

int heap_init(void)
{
	int ret = 0;

	tlsf_mem_kernel_pool = tlsf_create_with_pool((void *)KERNEL_HEAP_START, MAX_KERNEL_HEAP_SIZE);

	return ret;
}
