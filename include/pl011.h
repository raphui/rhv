#ifndef PL011_H
#define PL011_H

#include <stdint.h>

void pl011_write(uint8_t *buf, int size);
int pl011_init(void);

#endif /* PL011_H */
