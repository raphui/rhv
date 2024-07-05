#include <mmu.h>
#include <cache.h>
#include <system.h>
#include <printf.h>
#include <string.h>
#include <utils.h>
#include <mm/mm.h>
#include <sizes.h>

static uint64_t *get_ttb(void)
{
	return (uint64_t *)get_ttbr(current_el());
}

static void set_table(uint64_t *pt, uint64_t *table_addr)
{
	uint64_t val;

	val = PTE_TYPE_TABLE | (uint64_t)table_addr;
	*pt = val;
}

static uint64_t *alloc_pte(void)
{
	uint64_t *new_table = kmalloc_align(GRANULE_SIZE, GRANULE_SIZE);

	/* Mark all entries as invalid */
	memset(new_table, 0, GRANULE_SIZE);

	return new_table;
}

static uint64_t *find_pte(uint64_t addr)
{
	uint64_t *pte;
	uint64_t block_shift;
	uint64_t idx;
	int i;

	pte = get_ttb();

	for (i = 0; i < 4; i++) {
		block_shift = level2shift(i);
		idx = (addr & level2mask(i)) >> block_shift;
		pte += idx;

		if ((pte_type(pte) != PTE_TYPE_TABLE) || (block_shift <= GRANULE_SIZE_SHIFT))
			break;
		else
			pte = (uint64_t *)(*pte & XLAT_ADDR_MASK);
	}

	return pte;
}

#define MAX_PTE_ENTRIES 512

/* Splits a block PTE into table with subpages spanning the old block */
static void split_block(uint64_t *pte, int level)
{
	uint64_t old_pte = *pte;
	uint64_t *new_table;
	uint64_t i = 0;
	int levelshift;

	if ((*pte & PTE_TYPE_MASK) == PTE_TYPE_TABLE)
		return;

	/* level describes the parent level, we need the child ones */
	levelshift = level2shift(level + 1);

	new_table = alloc_pte();
	if (!new_table)
		printf("Unable to allocate PTE\n");


	for (i = 0; i < MAX_PTE_ENTRIES; i++) {
		new_table[i] = old_pte | (i << levelshift);

		/* Level 3 block PTEs have the table type */
		if ((level + 1) == 3)
			new_table[i] |= PTE_TYPE_TABLE;
	}

	/* Set the new table into effect */
	set_table(pte, new_table);
}

static void create_sections(uint64_t virt, uint64_t phys, uint64_t size,
			    uint64_t attr)
{
	uint64_t *ttb = get_ttb();
	uint64_t block_size;
	uint64_t block_shift;
	uint64_t *pte;
	uint64_t idx;
	uint64_t addr;
	uint64_t *table;
	uint64_t type;
	int level;

	addr = virt;

	attr &= ~PTE_TYPE_MASK;

	size = PAGE_ALIGN(size);

	while (size) {
		table = ttb;
		for (level = 0; level < 4; level++) {
			block_shift = level2shift(level);
			idx = (addr & level2mask(level)) >> block_shift;
			block_size = (1ULL << block_shift);

			pte = table + idx;

			if (size >= block_size && IS_ALIGNED(addr, block_size) &&
			    IS_ALIGNED(phys, block_size)) {
				type = (level == 3) ?
					PTE_TYPE_PAGE : PTE_TYPE_BLOCK;
				*pte = phys | attr | type;
				addr += block_size;
				phys += block_size;
				size -= block_size;
				break;
			} else {
				split_block(pte, level);
			}

			table = get_level_table(pte);
		}

	}

	tlb_invalidate();
}

static unsigned long get_pte_attrs(unsigned flags)
{
	switch (flags) {
	case MAP_CACHED:
		return CACHED_MEM;
	case MAP_UNCACHED:
		return attrs_uncached_mem();
	case MAP_FAULT:
		return 0x0;
	default:
		return ~0UL;
	}
}

void dma_inv_range(void *ptr, size_t size)
{
	unsigned long start = (unsigned long)ptr;
	unsigned long end = start + size;

	v8_inv_dcache_range(start, end);
}

void dma_flush_range(void *ptr, size_t size)
{
	unsigned long start = (unsigned long)ptr;
	unsigned long end = start + size;

	v8_flush_dcache_range(start, end);
}


int mmu_remap_range(void *virt_addr, void *phys_addr, uint64_t size, unsigned int flags)
{
	unsigned long attrs = get_pte_attrs(flags);

	if (attrs == ~0UL)
		return -EINVAL;

	create_sections((uint64_t)virt_addr, (uint64_t)phys_addr, (uint64_t)size, attrs);

//	if (flags == MAP_UNCACHED)
//		dma_inv_range(virt_addr, size);

	return 0;
}

static void init_range(size_t total_level0_tables)
{
	uint64_t *ttb = get_ttb();
	uint64_t addr = 0;

	while (total_level0_tables--) {
		printf("total_level0_tables: %d\n", total_level0_tables);
		printf("addr: 0x%llx\n", addr);
		printf("L0_XLAT_SIZE: 0x%llx\n", L0_XLAT_SIZE);
		printf("ttb: 0x%llx\n", ttb);
		mmu_remap_range((void *)addr, (void *)addr, L0_XLAT_SIZE, MAP_UNCACHED);
		split_block(ttb, 0);
		addr += L0_XLAT_SIZE;
		ttb++;
	}
}

void mmu_enable(void)
{
	uint64_t *ttb;
	int el;

	if (get_sctlr() & SCTLR_M) {
		ttb = (uint64_t *)get_ttb();
	} else {
		ttb = kmalloc_align(GRANULE_SIZE, SZ_16K);
	}

	el = current_el();
	set_ttbr_tcr_mair(el, (uint64_t)ttb, calc_tcr(el, BITS_PER_VA), MEMORY_ATTRIBUTES);
	if (el == 3)
		set_ttbr_tcr_mair(2, (uint64_t)ttb, calc_tcr(2, BITS_PER_VA), MEMORY_ATTRIBUTES);

	memset((void *)ttb, 0, GRANULE_SIZE);

	/*
	 * Assume maximum BITS_PER_PA set to 40 bits.
	 * Set 1:1 mapping of VA->PA. So to cover the full 1TB range we need 2 tables.
	 */
	init_range(2);

	mmu_remap_range((void *)PAGE_ALIGN_DOWN((uint64_t)_stext), (void *)PAGE_ALIGN_DOWN((uint64_t)_stext), PAGE_ALIGN((uint64_t)_etext - (uint64_t)_stext), MAP_CACHED);

	isb();
	set_sctlr(get_sctlr() | SCTLR_M | SCTLR_C | SCTLR_I);
}

void mmu_disable(void)
{
	unsigned int sctlr;

	sctlr = get_sctlr();
	sctlr &= ~(SCTLR_M | SCTLR_C);

	set_sctlr(sctlr);
	v8_flush_dcache_all();
	tlb_invalidate();

	dsb();
	isb();
}
