#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "dma.h"
#include "strops.h"

#define BUS_ADDRESS(addr)	(((addr) & ~0xC0000000) | 0xC0000000)

dma_channel channels[15];

dma_control_block dma_cb __attribute__((aligned(64)));

static uint16_t channel_map = 0x1F35;

uint32_t arm_to_phys(void *addr) {
    uint32_t addr32 = (uint32_t)(uintptr_t)addr;

    if(addr32 & 0xFE000000 == 0xFE000000) {
        // peripheral
        addr32 &= 0x00FFFFFF;
        addr32 |= 0x7E000000;
    } else if(addr32 & 0x30000000 == 0x30000000) {
        // gpu vram
    } else {
        // ram address
        addr32 &= 0x3FFFFFFF;
        addr32 |= 0xC0000000;
    }
    return addr32;
}

static uint16_t allocate_channel(uint32_t channel) {
    if (!(channel & ~0x0F)) {
        if (channel_map & (1 << channel)) {
            channel_map &= ~(1 << channel);
            return channel;
        }

        return -1;
    }

    uint16_t i = channel == CT_NORMAL ? 6 : 12;

    for (; i >= 0; i--) {
        if (channel_map & (1 << i)) {
            channel_map &= ~(1 << i);
            return i;
        }
    }

    return CT_NONE;
}

dma_channel *dma_open_channel(uint32_t channel) {
    uint32_t _channel = allocate_channel(channel);

    dma_channel *dma = (dma_channel *)&channels[_channel];
    dma->channel = _channel;

    dma->block = &dma_cb;
    dma->block->res[0] = 0;
    dma->block->res[1] = 0;

    uint32_t reg = mmio_read(DMA_GLOBAL_ENABLE);
    reg |= (1 << dma->channel);
    mmio_write(DMA_GLOBAL_ENABLE, reg);

    reg = mmio_read(DMA_CH_BASE(dma->channel) + DMA_CS);
    reg |= CS_RESET;
    mmio_write(DMA_CH_BASE(dma->channel) + DMA_CS, reg);

    while(mmio_read(DMA_CH_BASE(dma->channel) + DMA_CS) & CS_RESET);

    return dma;
}

void dma_close_channel(dma_channel *channel) {
    channel_map |= (1 << channel->channel);
}

void dma_setup_mem_copy(dma_channel *channel, void *dest, void *src, uint32_t length, uint32_t burst_length) {
    channel->block->transfer_info = (burst_length << TI_BURST_LENGTH_SHIFT)
						    | TI_SRC_WIDTH
						    | TI_SRC_INC
						    | TI_DEST_WIDTH
						    | TI_DEST_INC;

    channel->block->src_addr = arm_to_phys(src);
    channel->block->dest_addr = arm_to_phys(dest);
    channel->block->transfer_length = length;
    channel->block->mode_2d_stride = 0;
    channel->block->next_block_addr = 0;
}

void dma_start(dma_channel *channel) {
    uint32_t reg = mmio_read(DMA_CH_BASE(channel->channel) + DMA_CBA);
    reg = arm_to_phys(channel->block);
    mmio_write(DMA_CH_BASE(channel->channel) + DMA_CBA, reg);

    reg = mmio_read(DMA_CH_BASE(channel->channel) + DMA_CS);
    reg = CS_WAIT_FOR_OUTSTANDING_WRITES | (DEFAULT_PANIC_PRIORITY << CS_PANIC_PRIORITY_SHIFT) 
            | (DEFAULT_PRIORITY << CS_PRIORITY_SHIFT) | CS_ACTIVE;
    mmio_write(DMA_CH_BASE(channel->channel) + DMA_CS, reg);
}

uint8_t dma_wait(dma_channel *channel) {
    while(mmio_read(DMA_CH_BASE(channel->channel) + DMA_CS) & CS_ACTIVE) ;

    channel->status = mmio_read(DMA_CH_BASE(channel->channel) + DMA_CS) & CS_ERROR ? 0 : 1;

    return channel->status;

}