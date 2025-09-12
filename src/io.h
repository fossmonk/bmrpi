#include <stdint.h>
#include "hw.h"

#ifndef _UART_H_
#define _UART_H_

#define CHAR_TAB      (4444)
#define CHAR_UP_ARROW (5555)

void uart_init(void);
void uart_putc(unsigned char c);
int uart_getc(void);
int uart_getc_nb(void);
void uart_print(char *str);

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)(MMIO_BASE + reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t*)(MMIO_BASE + reg);
}

#endif /* _UART_H_ */