#include <stddef.h>
#include <stdint.h>

#ifndef _HW_H_
#define _HW_H_

/* MMIO */
#define MMIO_BASE   ((uintptr_t)0xFE000000)

/* GPIO */
#define GPIO_BASE    (0x200000)
#define GPFSEL0   (GPIO_BASE + 0x00)
#define GPSET0    (GPIO_BASE + 0x1C)
#define GPCLR0    (GPIO_BASE + 0x28)
#define GPPUPPDN0 (GPIO_BASE + 0xE4)

/* LED */ 
#define ACT_LED_PIN 47  // Pi 4 ACT LED GPIO pin

#define GPPUD        (GPIO_BASE + 0x94)
#define GPPUDCLK0    (GPIO_BASE + 0x98)

/* UART0 */
#define UART0_BASE      (GPIO_BASE + 0x1000)
#define UART0_DR        (UART0_BASE + 0x00)
#define UART0_RSRECR    (UART0_BASE + 0x04)
#define UART0_FR        (UART0_BASE + 0x18)
#define UART0_ILPR      (UART0_BASE + 0x20)
#define UART0_IBRD      (UART0_BASE + 0x24)
#define UART0_FBRD      (UART0_BASE + 0x28)
#define UART0_LCRH      (UART0_BASE + 0x2C)
#define UART0_CR        (UART0_BASE + 0x30)
#define UART0_IFLS      (UART0_BASE + 0x34)
#define UART0_IMSC      (UART0_BASE + 0x38)
#define UART0_RIS       (UART0_BASE + 0x3C)
#define UART0_MIS       (UART0_BASE + 0x40)
#define UART0_ICR       (UART0_BASE + 0x44)
#define UART0_DMACR     (UART0_BASE + 0x48)
#define UART0_ITCR      (UART0_BASE + 0x80)
#define UART0_ITIP      (UART0_BASE + 0x84)
#define UART0_ITOP      (UART0_BASE + 0x88)
#define UART0_TDR       (UART0_BASE + 0x8C)

/* UART3 */
#define UART3_BASE      (GPIO_BASE + 0x1600)
#define UART3_DR        (UART3_BASE + 0x00)
#define UART3_RSRECR    (UART3_BASE + 0x04)
#define UART3_FR        (UART3_BASE + 0x18)
#define UART3_ILPR      (UART3_BASE + 0x20)
#define UART3_IBRD      (UART3_BASE + 0x24)
#define UART3_FBRD      (UART3_BASE + 0x28)
#define UART3_LCRH      (UART3_BASE + 0x2C)
#define UART3_CR        (UART3_BASE + 0x30)
#define UART3_IFLS      (UART3_BASE + 0x34)
#define UART3_IMSC      (UART3_BASE + 0x38)
#define UART3_RIS       (UART3_BASE + 0x3C)
#define UART3_MIS       (UART3_BASE + 0x40)
#define UART3_ICR       (UART3_BASE + 0x44)
#define UART3_DMACR     (UART3_BASE + 0x48)
#define UART3_ITCR      (UART3_BASE + 0x80)
#define UART3_ITIP      (UART3_BASE + 0x84)
#define UART3_ITOP      (UART3_BASE + 0x88)
#define UART3_TDR       (UART3_BASE + 0x8C)

/* UART1 */
#define AUX_BASE        (GPIO_BASE + 0x15000)
#define AUX_ENABLE      (AUX_BASE + 0x04)
#define AUX_MU_IO_REG   (AUX_BASE + 0x40)
#define AUX_MU_IER_REG  (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG  (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG  (AUX_BASE + 0x4C)
#define AUX_MU_MCR_REG  (AUX_BASE + 0x50)
#define AUX_MU_LSR_REG  (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG (AUX_BASE + 0x60)
#define AUX_MU_BAUD_REG (AUX_BASE + 0x68)

/* MMIO READ/WRITE */
static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)(MMIO_BASE + reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t*)(MMIO_BASE + reg);
}

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