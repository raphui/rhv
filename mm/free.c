#include <mm/memory.h>

void kfree(void *mem)
{
	tlsf_free(tlsf_mem_kernel_pool, mem);
}
