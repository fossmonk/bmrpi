#include <stdint.h>
#include "hw.h"
#include "mbox.h"

volatile uint32_t mbox[36] __attribute__((aligned(16)));

int32_t mbox_call(uint8_t ch) {
    // Get the physical address of the mailbox buffer
    uint32_t mbox_phys = (uint32_t)(uintptr_t)mbox;

    // message address + channel number
    uint32_t r = ((mbox_phys & 0x3FFFFFFF) | (ch & 0xF));

    // wait until mailbox is not full
    while (mmio_read(MBOX_STATUS) & MBOX_FULL);

    // Write message address to mailbox
    mmio_write(MBOX_WRITE, r);

    while(1) {
        // wait until mailbox is not empty
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY);

        // check if response is for our message
        if (r == mmio_read(MBOX_READ)) {
            return mbox[1] == MBOX_VALID_RESP;
        }
    }
    return 0;
}

volatile uint32_t *mbox_get_buffer(void) {
    return mbox;
}