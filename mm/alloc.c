#include <stddef.h>
#include <string.h>
#include <mm/memory.h>

tlsf_t tlsf_mem_kernel_pool;

static void alloc(size_t size, unsigned int *m, tlsf_t pool)
{
	void *mem = NULL;

	mem = tlsf_malloc(pool, size);
	*m = (unsigned int)mem;
}

void *kmalloc(size_t size)
{
	unsigned int mem;

	alloc(size, &mem, tlsf_mem_kernel_pool);

	return (void *)mem;
}
