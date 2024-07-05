#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <system.h>
#include <pgtable.h>

extern unsigned long _stext[];
extern unsigned long _etext[];

#define PAGE_SIZE	4096

#define MAP_UNCACHED	0
#define MAP_CACHED	1
#define MAP_FAULT	2

#define MAP_DEFAULT	MAP_CACHED

#define CACHED_MEM      (PTE_BLOCK_MEMTYPE(MT_NORMAL) | \
			 PTE_BLOCK_OUTER_SHARE | \
			 PTE_BLOCK_AF)
#define UNCACHED_MEM    (PTE_BLOCK_MEMTYPE(MT_DEVICE_nGnRnE) | \
			 PTE_BLOCK_OUTER_SHARE | \
			 PTE_BLOCK_AF)

static inline unsigned long attrs_uncached_mem(void)
{
	unsigned long attrs = UNCACHED_MEM;

	switch (current_el()) {
	case 3:
	case 2:
		attrs |= PTE_BLOCK_UXN;
		break;
	default:
		attrs |= PTE_BLOCK_UXN | PTE_BLOCK_PXN;
		break;
	}

	return attrs;
}


static inline void tlb_invalidate(void)
{
	unsigned int el = current_el();

	dsb();

	if (el == 1)
		__asm__ __volatile__("tlbi vmalle1\n\t" : : : "memory");
	else if (el == 2)
		__asm__ __volatile__("tlbi alle2\n\t" : : : "memory");
	else if (el == 3)
		__asm__ __volatile__("tlbi alle3\n\t" : : : "memory");

	dsb();
	isb();
}

static inline void set_ttbr_tcr_mair(int el, uint64_t table, uint64_t tcr, uint64_t attr)
{
	dsb();
	if (el == 1) {
		asm volatile("msr ttbr0_el1, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el1, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el1, %0" : : "r" (attr) : "memory");
	} else if (el == 2) {
		asm volatile("msr ttbr0_el2, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el2, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el2, %0" : : "r" (attr) : "memory");
	} else if (el == 3) {
		asm volatile("msr ttbr0_el3, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el3, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el3, %0" : : "r" (attr) : "memory");
	} else {
		while (1)
			;
	}
	isb();
}

static inline uint64_t get_ttbr(int el)
{
	uint64_t val;
	if (el == 1) {
		asm volatile("mrs %0, ttbr0_el1" : "=r" (val));
	} else if (el == 2) {
		asm volatile("mrs %0, ttbr0_el2" : "=r" (val));
	} else if (el == 3) {
		asm volatile("mrs %0, ttbr0_el3" : "=r" (val));
	} else {
		while (1)
			;
	}

	return val;
}

static inline int level2shift(int level)
{
	/* Page is 12 bits wide, every level translates 9 bits */
	return (12 + 9 * (3 - level));
}

static inline uint64_t level2mask(int level)
{
	uint64_t mask = -EINVAL;

	if (level == 0)
		mask = L0_ADDR_MASK;
	else if (level == 1)
		mask = L1_ADDR_MASK;
	else if (level == 2)
		mask = L2_ADDR_MASK;
	else if (level == 3)
		mask = L3_ADDR_MASK;

	return mask;
}

/**
 * @brief Returns the TCR (Translation Control Register) value
 * 
 * @param el - Exception Level
 * @param va_bits - Virtual Address bits
 * @return uint64_t TCR
 */
static inline uint64_t calc_tcr(int el, int va_bits)
{
	uint64_t ips; // Intermediate Physical Address Size
	uint64_t tcr; // Translation Control Register

#if (BITS_PER_PA == 40)
	ips = 2;
#elif (BITS_PER_PA == 36)
	ips = 1;
#elif (BITS_PER_PA == 32)
	ips = 0;
#else
#error "Unsupported"
#endif

	if (el == 1)
		tcr = (ips << 32) | TCR_EPD1_DISABLE;
	else if (el == 2)
		tcr = (ips << 16);
	else
		tcr = (ips << 16);

	/* PTWs cacheable, inner/outer WBWA and inner shareable */
	tcr |= TCR_TG0_4K | TCR_SHARED_INNER | TCR_ORGN_WBWA | TCR_IRGN_WBWA;
	tcr |= TCR_T0SZ(va_bits);

	return tcr;
}

static inline int pte_type(uint64_t *pte)
{
	return *pte & PTE_TYPE_MASK;
}

static inline uint64_t *get_level_table(uint64_t *pte)
{
	uint64_t *table = (uint64_t *)(*pte & XLAT_ADDR_MASK);

	if (pte_type(pte) != PTE_TYPE_TABLE) {
		while (1)
			;
	}

	return table;
}

void dma_inv_range(void *ptr, size_t size);
void dma_flush_range(void *ptr, size_t size);

void mmu_enable(void);
void mmu_disable(void);

int mmu_remap_range(void *virt_addr, void *phys_addr, uint64_t size, unsigned int flags);

#endif /* MMU_H */
