#ifndef SYSTEM_H
#define SYSTEM_H


#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb sy" : : : "memory")
#define dmb() __asm__ __volatile__ ("dmb sy" : : : "memory")

/*
 * SCTLR bits
 */
#define SCTLR_M    (1 << 0)	/* MMU enable				*/
#define SCTLR_A    (1 << 1)	/* Alignment abort enable		*/
#define SCTLR_C    (1 << 2)	/* Dcache enable			*/
#define SCTLR_I    (1 << 12)	/* Icache enable			*/

static inline unsigned long read_mpidr(void)
{
	unsigned long val;

	asm volatile("mrs %0, mpidr_el1" : "=r" (val));

	return val;
}

static inline void set_cntfrq(unsigned long cntfrq)
{
	asm volatile("msr cntfrq_el0, %0" : : "r" (cntfrq) : "memory");
}

static inline unsigned long get_cntfrq(void)
{
	unsigned long cntfrq;

	asm volatile("mrs %0, cntfrq_el0" : "=r" (cntfrq));
	return cntfrq;
}

static inline unsigned long get_cntpct(void)
{
	unsigned long cntpct;

	isb();
	asm volatile("mrs %0, cntpct_el0" : "=r" (cntpct));

	return cntpct;
}

static inline unsigned int current_el(void)
{
	unsigned int el;
	asm volatile("mrs %0, CurrentEL" : "=r" (el) : : "cc");
	return el >> 2;
}

static inline unsigned int get_sctlr(void)
{
	unsigned int val;

	unsigned int el = current_el();
	if (el == 1)
		asm volatile("mrs %0, sctlr_el1" : "=r" (val) : : "cc");
	else if (el == 2)
		asm volatile("mrs %0, sctlr_el2" : "=r" (val) : : "cc");
	else
		asm volatile("mrs %0, sctlr_el3" : "=r" (val) : : "cc");

	return val;
}

static inline void set_sctlr(unsigned int val)
{
	unsigned int el;

	el = current_el();
	if (el == 1)
		asm volatile("msr sctlr_el1, %0" : : "r" (val) : "cc");
	else if (el == 2)
		asm volatile("msr sctlr_el2, %0" : : "r" (val) : "cc");
	else
		asm volatile("msr sctlr_el3, %0" : : "r" (val) : "cc");
	isb();
}

#endif /* SYSTEM_H */
