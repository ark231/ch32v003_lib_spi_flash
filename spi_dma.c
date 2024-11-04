#include "spi_dma.h"

#include <ch32v003fun.h>

void spi_dma_write_init(uint32_t msize, uint32_t psize) {
    DMA1_Channel3->CFGR ^= DMA_CFGR3_MSIZE | DMA_CFGR3_PSIZE | DMA_CFGR3_DIR;

    DMA1_Channel3->CFGR |= msize;
    DMA1_Channel3->CFGR |= psize;
    DMA1_Channel3->CFGR |= DMA_DIR_PeripheralDST;
}
void spi_dma_read_init(uint32_t msize, uint32_t psize) {
    DMA1_Channel2->CFGR ^= DMA_CFGR2_MSIZE | DMA_CFGR2_PSIZE | DMA_CFGR2_DIR;

    DMA1_Channel2->CFGR |= msize;
    DMA1_Channel2->CFGR |= psize;
    DMA1_Channel2->CFGR |= DMA_DIR_PeripheralSRC;
}

void spi_dma_common_write(void* src, uint16_t len, uint32_t priority) {
    static uint32_t devzero = 0;
    DMA1_Channel3->CNTR     = len;
    if (src != NULL) {
        DMA1_Channel3->MADDR = (uint32_t)src;  // the power of C HAHAHA
        DMA1_Channel3->CFGR |= DMA_MemoryInc_Enable;
    } else {
        DMA1_Channel3->MADDR = (uint32_t)(&devzero);
        DMA1_Channel3->CFGR ^= DMA_CFGR3_MINC;  // disable auto increment
    }
    DMA1_Channel3->PADDR = (uint32_t)(&SPI1->DATAR);
    DMA1_Channel3->CNTR  = len;
}

void spi_dma_write_single_shot(void* src, uint16_t len, uint32_t priority) {
    spi_dma_common_write(src, len, priority);
    DMA1_Channel3->CFGR ^= DMA_CFGR3_CIRC;  // disable circular mode
    DMA1_Channel3->CFGR |= DMA_CFGR3_EN;
}
void spi_dma_write_start_cyclic(void* src, uint16_t len, uint32_t priority) {
    spi_dma_common_write(src, len, priority);
    DMA1_Channel3->CFGR |= DMA_Mode_Circular;
    DMA1_Channel3->CFGR |= DMA_CFGR3_EN;
}
void spi_dma_write_continue_cyclic() { DMA1_Channel3->CFGR |= DMA_CFGR3_EN; }
void spi_dma_write_end_cyclic() { DMA1_Channel3->CFGR ^= DMA_CFGR3_CIRC; }
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

void spi_dma_common_read(void* dst, uint16_t len, uint32_t priority) {
    static uint32_t devnull = 0;
    DMA1_Channel2->CNTR     = len;
    if (dst != NULL) {
        DMA1_Channel2->MADDR = (uint32_t)dst;  // the power of C HAHAHA
        DMA1_Channel2->CFGR |= DMA_MemoryInc_Enable;
    } else {
        DMA1_Channel2->MADDR = (uint32_t)(&devnull);
        DMA1_Channel2->CFGR ^= DMA_CFGR2_MINC;  // disable auto increment
    }
    DMA1_Channel2->PADDR = (uint32_t)(&SPI1->DATAR);
    DMA1_Channel2->CNTR  = len;
}

void spi_dma_read_single_shot(void* dst, uint16_t len, uint32_t priority) {
    spi_dma_common_read(dst, len, priority);
    DMA1_Channel2->CFGR ^= DMA_CFGR2_CIRC;  // disable circular mode
    DMA1_Channel2->CFGR |= DMA_CFGR2_EN;
}
void spi_dma_read_start_cyclic(void* dst, uint16_t len, uint32_t priority) {
    spi_dma_common_read(dst, len, priority);
    DMA1_Channel2->CFGR |= DMA_Mode_Circular;
    DMA1_Channel2->CFGR |= DMA_CFGR2_EN;
}
void spi_dma_read_continue_cyclic() { DMA1_Channel2->CFGR |= DMA_CFGR2_EN; }
void spi_dma_read_end_cyclic() { DMA1_Channel2->CFGR ^= DMA_CFGR2_CIRC; }
spi_dma_status_t spi_dma_read_status() {
    if (DMA1->INTFR & DMA_TEIF2) {
        DMA1->INTFCR &= DMA_CTEIF2;
        return DMA_ERROR;
    }
    if (DMA1->INTFR & DMA_TCIF2) {
        DMA1->INTFCR &= DMA_CTCIF2;
        return DMA_FINISHED;
    }
    return DMA_RUNNING;
}
