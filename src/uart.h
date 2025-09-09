#include <stdint.h>
#include "hw.h"
#include "mmio.h"

#ifndef _UART_H_
#define _UART_H_

void uart_init(void);
void uart_putc(unsigned char c);
uint8_t uart_getc(void);


#endif /* _UART_H_ */