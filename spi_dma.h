#ifndef CH32V003_LIBRARY_SPI_FLASH_SPI_DMA
#define CH32V003_LIBRARY_SPI_FLASH_SPI_DMA
#include <ch32v003fun.h>
#include <stddef.h>
typedef enum {
    SPI_OK,
    SPI_ERROR,
} spi_err_t;

typedef enum {
    DMA_RUNNING,
    DMA_FINISHED,
    DMA_ERROR,
} spi_dma_status_t;

// ch32v003fun has these defines
//
// DMA_MemoryDataSize_*
// DMA_PeripheralDataSize_*
// DMA_Priority_*
void spi_dma_write_init(uint32_t msize, uint32_t psize);
void spi_dma_write_single_shot(void* src, uint16_t len, uint32_t priority);
void spi_dma_write_start_cyclic(void* src, uint16_t len, uint32_t priority);
void spi_dma_write_continue_cyclic(void);
void spi_dma_write_end_cyclic(void);
spi_dma_status_t spi_dma_write_status(void);

void spi_dma_read_init(uint32_t msize, uint32_t psize);
void spi_dma_read_single_shot(void* dst, uint16_t len, uint32_t priority);
void spi_dma_read_start_cyclic(void* dst, uint16_t len, uint32_t priority);
void spi_dma_read_continue_cyclic(void);
void spi_dma_read_end_cyclic(void);
spi_dma_status_t spi_dma_read_status(void);
#endif
