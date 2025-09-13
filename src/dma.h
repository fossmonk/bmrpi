#include <stdint.h>

#ifndef _DMA_H_
#define _DMA_H_

#define CS_RESET			            (1 << 31)
#define CS_ABORT			            (1 << 30)
#define CS_WAIT_FOR_OUTSTANDING_WRITES	(1 << 28)
#define CS_PANIC_PRIORITY_SHIFT		    20
#define DEFAULT_PANIC_PRIORITY		    15
#define CS_PRIORITY_SHIFT		        16
#define DEFAULT_PRIORITY		        1
#define CS_ERROR			            (1 << 8)
#define CS_INT				            (1 << 2)
#define CS_END				            (1 << 1)
#define CS_ACTIVE			            (1 << 0)

#define TI_PERMAP_SHIFT		   16
#define TI_BURST_LENGTH_SHIFT  12
#define DEFAULT_BURST_LENGTH   0
#define TI_SRC_IGNORE		  (1 << 11)
#define TI_SRC_DREQ			  (1 << 10)
#define TI_SRC_WIDTH		  (1 << 9)
#define TI_SRC_INC			  (1 << 8)
#define TI_DEST_DREQ		  (1 << 6)
#define TI_DEST_WIDTH		  (1 << 5)
#define TI_DEST_INC			  (1 << 4)
#define TI_WAIT_RESP		  (1 << 3)
#define TI_TDMODE			  (1 << 1)
#define TI_INTEN			  (1 << 0)

#define DMA_CH_BASE(channel) (0x00007000 + (channel * 0x100))
#define DMA_INT_STATUS (0x00007FE0)
#define DMA_GLOBAL_ENABLE (0x00007FF0)
#define DMA_CS  (0x00)
#define DMA_CBA (0x04)

typedef struct {
    uint32_t transfer_info;
    uint32_t src_addr;
    uint32_t dest_addr;
    uint32_t transfer_length;
    uint32_t stride;
    uint32_t next_block_addr;
    uint32_t res[2];
} dma_control_block;

typedef struct {
    uint32_t control;
    uint32_t control_block_addr;
    dma_control_block block;
} dma_channel_regs;

typedef struct {
    uint32_t channel;
    dma_control_block *block;
    uint8_t status;
} dma_channel;

typedef enum {
    CT_NONE = -1,
    CT_NORMAL = 0x81
} dma_channel_type;

dma_channel *dma_open_channel(uint32_t channel);
void dma_close_channel(dma_channel *channel);
void dma_setup_mem_copy(dma_channel *channel, void *dest, void *src, uint32_t length, uint32_t burst_length);
void dma_setup_2dmem_copy(dma_channel *channel, 
                          void *dest, void *src, 
                          uint32_t width_in_bytes, 
                          uint32_t height_in_pixels,
                          uint32_t pitch,
                          uint32_t burst_length);
void dma_start(dma_channel *channel);
uint8_t dma_wait(dma_channel *channel);

#endif /* _DMA_H_ */