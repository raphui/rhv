/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CACHE_H
#define CACHE_H

void v8_invalidate_icache_all(void);
void v8_flush_dcache_all(void);
void v8_invalidate_dcache_all(void);
void v8_flush_dcache_range(unsigned long start, unsigned long end);
void v8_inv_dcache_range(unsigned long start, unsigned long end);

static inline void icache_invalidate(void)
{
	v8_invalidate_icache_all();
}

#endif /* CACHE_H */
