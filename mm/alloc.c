#include <stddef.h>
#include <mm/memory.h>

tlsf_t tlsf_mem_kernel_pool;

void *kmalloc(size_t size)
{
	return tlsf_malloc(tlsf_mem_kernel_pool, size);
}

void *kmalloc_align(size_t align, size_t size)
{
	return tlsf_memalign(tlsf_mem_kernel_pool, align, size);
}
