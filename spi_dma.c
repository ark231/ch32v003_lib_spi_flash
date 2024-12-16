#include "spi_dma.h"

#include <ch32v003fun.h>
#include <inttypes.h>

void spi_dma_enable_tx(void) { SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx; }
void spi_dma_disable_tx(void) { SPI1->CTLR2 &= ~SPI_I2S_DMAReq_Tx; }
void spi_dma_enable_rx(void) { SPI1->CTLR2 |= SPI_I2S_DMAReq_Rx; }
void spi_dma_disable_rx(void) { SPI1->CTLR2 &= ~SPI_I2S_DMAReq_Rx; }

void spi_dma_write_init(uint32_t mwidth, uint32_t pwidth) {
    DMA1_Channel3->CFGR &= ~(DMA_CFGR3_MSIZE | DMA_CFGR3_PSIZE | DMA_CFGR3_DIR);

    DMA1_Channel3->CFGR |= mwidth;
    DMA1_Channel3->CFGR |= pwidth;
    DMA1_Channel3->CFGR |= DMA_DIR_PeripheralDST;
}

void spi_dma_common_write(void* src, uint16_t len, uint32_t priority, bool auto_inc) {
    DMA1_Channel3->CNTR  = len;
    DMA1_Channel3->MADDR = (uint32_t)src;  // the power of C HAHAHA
    if (auto_inc) {
        DMA1_Channel3->CFGR |= DMA_MemoryInc_Enable;
    } else {
        DMA1_Channel3->CFGR &= ~DMA_CFGR3_MINC;  // disable auto increment
    }
    DMA1_Channel2->CFGR &= ~DMA_CFGR2_PINC;
    DMA1_Channel3->PADDR = (uint32_t)(&SPI1->DATAR);
    DMA1_Channel3->CNTR  = len;
}

void spi_dma_write_single_shot(void* src, uint16_t len, uint32_t priority, bool auto_inc) {
    spi_dma_common_write(src, len, priority, auto_inc);
    DMA1_Channel3->CFGR &= ~DMA_CFGR3_CIRC;  // disable circular mode
    DMA1_Channel3->CFGR |= DMA_CFGR3_EN;
}
void spi_dma_write_start_circular(void* src, uint16_t len, uint32_t priority, bool auto_inc) {
    spi_dma_common_write(src, len, priority, auto_inc);
    DMA1_Channel3->CFGR |= DMA_Mode_Circular;
    DMA1_Channel3->CFGR |= DMA_CFGR3_EN;
}
void spi_dma_write_continue_circular() { DMA1_Channel3->CFGR |= DMA_CFGR3_EN; }
void spi_dma_write_end_circular() { DMA1_Channel3->CFGR &= ~DMA_CFGR3_CIRC; }
spi_dma_status_t spi_dma_write_status() {
    if (DMA1->INTFR & DMA_TEIF3) {
        DMA1->INTFCR &= DMA_CTEIF3;
        return DMA_ERROR;
    }
    if (DMA1->INTFR & DMA_TCIF3) {
        DMA1->INTFCR &= DMA_CTCIF3;
        return DMA_FINISHED;
    }
    return DMA_RUNNING;
}

void spi_dma_common_read(void* dst, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth, uint32_t pwidth) {
    RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
    DMA1_Channel2->PADDR = (uint32_t)(&SPI1->DATAR);
    DMA1_Channel2->MADDR = (uint32_t)dst;  // the power of C HAHAHA
    DMA1_Channel2->CNTR  = len;
    DMA1_Channel2->CFGR |= priority;

    DMA1_Channel2->CFGR &= ~(DMA_CFGR2_MSIZE | DMA_CFGR2_PSIZE | DMA_CFGR2_DIR);
    DMA1_Channel2->CFGR |= mwidth;
    DMA1_Channel2->CFGR |= pwidth;
    DMA1_Channel2->CFGR |= DMA_DIR_PeripheralSRC;
    if (auto_inc) {
        DMA1_Channel2->CFGR |= DMA_MemoryInc_Enable;
    } else {
        DMA1_Channel2->CFGR &= ~DMA_CFGR2_MINC;  // disable auto increment
    }
    DMA1_Channel2->CFGR &= ~DMA_CFGR2_PINC;

    DMA1_Channel2->CFGR |= DMA_CFGR2_TCIE;

    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    spi_dma_read_is_running = true;
}

void spi_dma_read_single_shot(void* dst, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                              uint32_t pwidth) {
    spi_dma_enable_rx();
    DMA1_Channel2->CFGR &= ~DMA_CFGR2_EN;
    spi_dma_common_read(dst, len, priority, auto_inc, mwidth, pwidth);
    DMA1_Channel2->CFGR &= ~DMA_CFGR2_CIRC;  // disable circular mode
    DMA1_Channel2->CFGR |= DMA_CFGR2_EN;
}
void spi_dma_read_start_circular(void* dst, uint16_t len, uint32_t priority, bool auto_inc, uint32_t mwidth,
                                 uint32_t pwidth) {
    spi_dma_common_read(dst, len, priority, auto_inc, mwidth, pwidth);
    DMA1_Channel2->CFGR |= DMA_Mode_Circular;
    DMA1_Channel2->CFGR |= DMA_CFGR2_EN;
}
void spi_dma_read_continue_circular() { DMA1_Channel2->CFGR |= DMA_CFGR2_EN; }
void spi_dma_read_end_circular() { DMA1_Channel2->CFGR &= ~DMA_CFGR2_CIRC; }
spi_dma_status_t spi_dma_read_status() {
    spi_dma_status_t result = DMA_RUNNING;
    // latter condition has higher priority
    if (DMA1->INTFR & DMA_HTIF2) {
        DMA1->INTFCR |= DMA_CHTIF2;
        DMA1->INTFCR |= DMA_CGIF2;
        result = DMA_RUNNING;
    }
    if (DMA1->INTFR & DMA_TCIF2) {
        DMA1->INTFCR |= DMA_CTCIF2;
        DMA1->INTFCR |= DMA_CGIF2;
        result = DMA_FINISHED;
    }
    if (DMA1->INTFR & DMA_TEIF2) {
        DMA1->INTFCR |= DMA_CTEIF2;
        DMA1->INTFCR |= DMA_CGIF2;
        result = DMA_ERROR;
    }
    return result;
}
volatile bool spi_dma_read_is_running = false;
