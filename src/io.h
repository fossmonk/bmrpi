#include <stdint.h>
#include "hw.h"

#ifndef _UART_H_
#define _UART_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

void uart_init(void);
void uart_putc(unsigned char c);
int uart_getc(void);
int uart_getc_nb(void);
void uart_print(char *str);
void wait_msec(uint32_t n);

/* UART3 */
void uart3_init(void);
void uart3_putc(uint8_t c);
int uart3_getc(void);
int uart3_getc_nb(void);

#endif /* _UART_H_ */