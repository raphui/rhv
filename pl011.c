#include <pl011.h>
#include <stdint.h>

typedef struct __pl011 {
	volatile uint32_t DR;
	volatile uint32_t RSR;
	volatile uint32_t reserved[4];
	volatile uint32_t FR;
	volatile uint32_t reserved1;
	volatile uint32_t ILPR;
	volatile uint32_t IBRD;
	volatile uint32_t FBRD;
	volatile uint32_t LCR_H;
	volatile uint32_t CR;
	volatile uint32_t IFLS;
	volatile uint32_t IMSC;
	volatile uint32_t RIS;
	volatile uint32_t MIS;
	volatile uint32_t ICR;

} PL011_t;

#define CR_UARTEN (1 << 0)
#define CR_TXE (1 << 8)
#define CR_RXE (1 << 9)

#define LCR_H_PEN (1 << 1)
#define LCR_H_EPS (1 << 2)
#define LCR_H_STP2 (1 << 3)
#define LCR_H_WLEN(x) (x << 5)

#define FR_BUSY (1 << 3)

#define PL011_BASE_ADDRESS 0x9000000
#define APB_CLOCK_SPEED 24000000
#define BAUDRATE 115200

static void wait_busy(void)
{
	PL011_t *pl011 = (PL011_t *)PL011_BASE_ADDRESS;

	while (pl011->FR & FR_BUSY)
		;
}

static void calculate_divisiors(uint32_t *integer, uint32_t *fractional)
{
	//64 * F_UARTCLK / (16 * B) = 4 * F_UARTCLK / B
	const uint32_t div = 4 * APB_CLOCK_SPEED / BAUDRATE;

	*fractional = div & 0x3f;
	*integer = (div >> 6) & 0xffff;
}

void pl011_write(uint8_t *buf, int size)
{
	PL011_t *pl011 = (PL011_t *)PL011_BASE_ADDRESS;

	wait_busy();

	for (int i = 0; i < size; i++) {
		pl011->DR = buf[i];

		wait_busy();
	}
}

int pl011_init(void)
{
	uint32_t int_baud_rate;
	uint32_t frac_baud_rate;
	PL011_t *pl011 = (PL011_t *)PL011_BASE_ADDRESS;

	pl011->CR &= ~CR_UARTEN;

	calculate_divisiors(&int_baud_rate, &frac_baud_rate);

	pl011->IBRD = int_baud_rate;
	pl011->FBRD = frac_baud_rate;

	pl011->LCR_H = LCR_H_WLEN(0x3); // 8 bits word length

	pl011->CR |= CR_UARTEN | CR_TXE;

	return 0;
}
