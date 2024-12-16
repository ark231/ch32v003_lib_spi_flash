#ifndef CH32V003_LIBRARY_SPI_FLASH_SPI_DMA
#define CH32V003_LIBRARY_SPI_FLASH_SPI_DMA
#include <ch32v003fun.h>
#include <stddef.h>
typedef enum {
    SPI_OK,
    SPI_ERROR,
} spi_err_t;

typedef enum {
    DMA_RUNNING,  // including halfway
    DMA_FINISHED,
    DMA_ERROR,
} spi_dma_status_t;

// ch32v003fun has these defines
//
// DMA_MemoryDataSize_*
// DMA_PeripheralDataSize_*
// DMA_Priority_*
void spi_dma_enable_tx(void);
void spi_dma_disable_tx(void);

/**
 * @brief start dma write with spi
 * @note interrupt handler for ending transmission is required
 *
 * @param mwidth memory address data width
 * @param pwidthperipheral data width
 */
void spi_dma_write_single_shot(void* src, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                               uint32_t pwidth);
void spi_dma_write_start_circular(void* src, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                                  uint32_t pwidth);
void spi_dma_write_continue_circular(void);
void spi_dma_write_end_circular(void);
spi_dma_status_t spi_dma_write_status(void);
bool spi_dma_write_is_running(void);
void spi_dma_on_write_completed(void);

void spi_dma_enable_rx(void);
void spi_dma_disable_rx(void);

void spi_dma_read_single_shot(void* dst, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                              uint32_t pwidth);
void spi_dma_read_start_circular(void* dst, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                                 uint32_t pwidth);
void spi_dma_read_continue_circular(void);
void spi_dma_read_end_circular(void);
spi_dma_status_t spi_dma_read_status(void);
bool spi_dma_read_is_running(void);
void spi_dma_on_read_completed(void);
#endif
