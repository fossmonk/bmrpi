#include <stdint.h>
#include "hw.h"
#include "io.h"

#define GPIO_MAX_PIN (53)
#define GPIO_FUNCTION_ALT5 (2)
#define GPIO_PULL_NONE (0)

uint32_t gpio_call(uint32_t pin_number, 
                   uint32_t value, 
                   uint32_t base, 
                   uint32_t field_size, 
                   uint32_t field_max) {
    uint32_t field_mask = (1 << field_size) - 1;
  
    if (pin_number > field_max) return 0;
    if (value > field_mask) return 0; 

    uint32_t num_fields = 32 / field_size;
    uint32_t reg = base + ((pin_number / num_fields) * 4);
    uint32_t shift = (pin_number % num_fields) * field_size;

    uint32_t curval = mmio_read(reg);
    curval &= ~(field_mask << shift);
    curval |= value << shift;
    mmio_write(reg, curval);

    return 1;
}

uint32_t gpio_set     (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPSET0, 1, GPIO_MAX_PIN); }
uint32_t gpio_clear   (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPCLR0, 1, GPIO_MAX_PIN); }
uint32_t gpio_pull    (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
uint32_t gpio_function(uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPFSEL0, 3, GPIO_MAX_PIN); }

void gpio_use_as_alt5(uint32_t pin_number) {
    gpio_pull(pin_number, GPIO_PULL_NONE);
    gpio_function(pin_number, GPIO_FUNCTION_ALT5);
}

void uart_init() {
    // enable UART1
    mmio_write(AUX_ENABLE, 1);
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); //disable interruptsmmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3); //8 bits
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); //disable interrupts
    mmio_write(AUX_MU_BAUD_REG, 541);
    gpio_use_as_alt5(14);
    gpio_use_as_alt5(15);
    mmio_write(AUX_MU_CNTL_REG, 3);
}

void uart_putc(unsigned char c) {
    // Wait until the UART's transmit FIFO is empty
    while(!(mmio_read(AUX_MU_LSR_REG) & (1 << 5)));
    
    // Write the character to the UART's transmit register
    mmio_write(AUX_MU_IO_REG, c);
}

int uart_getc() {
    // Wait until the UART's receive FIFO has data
    while(!(mmio_read(AUX_MU_LSR_REG) & 1));
    
    // Read the character from the UART's receive register
    return mmio_read(AUX_MU_IO_REG);
}

int uart_getc_nb() {
    // Wait until the UART's receive FIFO has data
    if(!(mmio_read(AUX_MU_LSR_REG) & 1)) return -1;
    
    // Read the character from the UART's receive register
    return mmio_read(AUX_MU_IO_REG);
}

void uart_print(char *str) {
    if(str != NULL) {
        while(*str) uart_putc(*str++);
    }
}
