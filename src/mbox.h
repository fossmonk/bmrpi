#include <stdint.h>

#ifndef _MBOX_H_
#define _MBOX_H_

/* MAILBOX */
#define MBOX_BASE    (0xB880)
#define MBOX_READ    (MBOX_BASE + 0x00)
#define MBOX_STATUS  (MBOX_BASE + 0x18)
#define MBOX_WRITE   (MBOX_BASE + 0x20)

#define MBOX_CH_PROP (8)
#define MBOX_FULL    (0x80000000)
#define MBOX_EMPTY   (0x40000000)

#define MBOX_VALID_RESP (0x80000000)

int32_t mbox_call(uint8_t ch);
volatile uint32_t *mbox_get_buffer(void);

#endif