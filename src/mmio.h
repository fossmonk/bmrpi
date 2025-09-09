#include <stdint.h>
#include "hw.h"

#ifndef _MMIO_H_
#define _MMIO_H_

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t*)(uintptr_t)(MMIO_BASE + reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t*)(uintptr_t)(MMIO_BASE + reg);
}

#endif /* _MMIO_H_ */