#include <stdint.h>
#include "hw.h"
#include "io.h"
#include "led.h"

#define GPFSEL4 (GPIO_BASE + 0x10)
#define GPSET1  (GPIO_BASE + 0x20)
#define GPCLR1  (GPIO_BASE + 0x2C)

void led_init() {
    // Set ACT LED pin as output (GPIO 47 = bit 21 in GPFSEL4)
    uint32_t val = mmio_read(GPFSEL4);
    val &= ~(7 << 21);
    val |= (1 << 21);  // Set as output
    mmio_write(GPFSEL4, val);
}

void led_on() {
    mmio_write(GPSET1, (1 << (ACT_LED_PIN - 32)));
}

void led_off() {
    mmio_write(GPCLR1, (1 << (ACT_LED_PIN - 32)));
}

void led_blink_test(int tries) {
    while(tries--) {
        led_on();
        delay(750000000);
        led_off();
        delay(750000000);
    }
}