#ifndef MEMORY_H
#define MEMORY_H

#define KERNEL_HEAP_START	0x50000000
#define KERNEL_HEAP_END		0x60000000
#define MAX_KERNEL_HEAP_SIZE	(KERNEL_HEAP_END - KERNEL_HEAP_START)

#include <tlsf.h>

extern tlsf_t tlsf_mem_kernel_pool;
extern tlsf_t tlsf_mem_user_pool;

#endif /* MEMORY_H */
