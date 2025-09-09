#include <stdint.h>
#include "hw.h"
#include "led.h"

void led_init() {
    // Set ACT LED pin as output (GPIO 47 = bit 21 in GPFSEL4)
    volatile uint32_t* gpfsel4 = (uint32_t*)(GPIO_BASE + 0x10);
    uint32_t val = *gpfsel4;
    val &= ~(7 << 21);
    val |= (1 << 21);  // Set as output
    *gpfsel4 = val;
}

void led_on() {
    volatile uint32_t* gpset1 = (uint32_t*)(GPIO_BASE + 0x20);
    *gpset1 = (1 << (ACT_LED_PIN - 32));
}

void led_off() {
    volatile uint32_t* gpclr1 = (uint32_t*)(GPIO_BASE + 0x2C);
    *gpclr1 = (1 << (ACT_LED_PIN - 32));
}

void led_blink_test(int tries) {
    while(tries--) {
        led_on();
        sw_delay(2);
        led_off();
        sw_delay(2);
    }
}