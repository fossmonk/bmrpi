#include <stddef.h>
#include <stdint.h>

#ifndef _HW_H_
#define _HW_H_

/* MMIO */
#define MMIO_BASE   (0xFE000000)

/* GPIO */
#define GPIO_BASE    (0x200000)
#define GPFSEL1   (GPIO_BASE + 0x04)
#define GPSET0    (GPIO_BASE + 0x1C)
#define GPCLR0    (GPIO_BASE + 0x28)

/* LED */ 
#define ACT_LED_PIN 47  // Pi 4 ACT LED GPIO pin

#define GPPUD        (GPIO_BASE + 0x94)
#define GPPUDCLK0    (GPIO_BASE + 0x98)

/* UART */
#define UART0_BASE   (GPIO_BASE + 0x1000)
#define UART0_DR     (UART0_BASE + 0x00)
#define UART0_RSRECR (UART0_BASE + 0x04)
#define UART0_FR     (UART0_BASE + 0x18)
#define UART0_ILPR   (UART0_BASE + 0x20)
#define UART0_IBRD   (UART0_BASE + 0x24)
#define UART0_FBRD   (UART0_BASE + 0x28)
#define UART0_LCRH   (UART0_BASE + 0x2C)
#define UART0_CR     (UART0_BASE + 0x30)
#define UART0_IFLS   (UART0_BASE + 0x34)
#define UART0_IMSC   (UART0_BASE + 0x38)
#define UART0_RIS    (UART0_BASE + 0x3C)
#define UART0_MIS    (UART0_BASE + 0x40)
#define UART0_ICR    (UART0_BASE + 0x44)
#define UART0_DMACR  (UART0_BASE + 0x48)
#define UART0_ITCR   (UART0_BASE + 0x80)
#define UART0_ITIP   (UART0_BASE + 0x84)
#define UART0_ITOP   (UART0_BASE + 0x88)
#define UART0_TDR    (UART0_BASE + 0x8C)

/* MAILBOX */
#define MBOX_BASE    (0xB880)
#define MBOX_READ    (MBOX_BASE + 0x00)
#define MBOX_STATUS  (MBOX_BASE + 0x18)
#define MBOX_WRITE   (MBOX_BASE + 0x20)

#define MBOX_CH_PROP (8)
#define MBOX_FULL    (0x80000000)
#define MBOX_EMPTY   (0x40000000)

/* HW DELAY */
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}

/* SW DELAY */
static inline void sw_delay(int32_t count){
	while(count--) {
		int x = 1000000;
		while(x--) {
			int y = 100;
			while(y--);
		}
	}
}

#endif /* _HW_H_ */