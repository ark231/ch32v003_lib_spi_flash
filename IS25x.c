#include "IS25x.h"

#include <stdio.h>

#include "spi_dma.h"
#include "spi_helper.h"

void is25x_init(IS25x *self, uint8_t cs) {
    self->cs = cs;
    funPinMode(cs, FUN_OUTPUT);
}

uint8_t dummy_tx;

void SPI1_IRQHandler(void) __attribute__((interrupt));  // NOLINT(readability-identifier-naming)
void SPI1_IRQHandler(void) {
    uint16_t stat = SPI1->STATR;
    if (stat & SPI_STATR_TXE) {
        SPI_write_8(dummy_tx);
    } else if (stat & SPI_STATR_RXNE) {
        SPI_read_8();
    }
}

void is25x_read_no_dma(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {
    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_NORD);
    SPI_wait_not_busy();
    spi_transfar_24be_no_cs(addr);
    SPI_wait_not_busy();
    spi_set_2line_rxonly();
    for (size_t i = 0; i < len; i++) {
        dst[i] = spi_read_8_no_tx();
    }
    if (SPI1->STATR & SPI_STATR_RXNE) {
        SPI_read_8();
    }
    spi_set_2line_fullduplex();

    spi_cs_high(self->cs);
    // HACK: it seems one byte is left in the rx buffer causing issue later, so read it and problem solved. I obviously
    // DON'T KNOW  WHY.
    if (SPI1->STATR & SPI_STATR_RXNE) {
        SPI_read_8();
    }
}

void is25x_begin_dma_read(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {
    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_NORD);
    spi_transfar_24be_no_cs(addr);
    spi_set_2line_rxonly();

    spi_dma_read_single_shot(dst, len, DMA_Priority_VeryHigh, true, DMA_MemoryDataSize_Byte, DMA_MemoryDataSize_Byte);
}
bool is25x_dma_read_is_completed(IS25x *self) { return spi_dma_read_status() == DMA_FINISHED; }
void is25x_end_dma_read(IS25x *self) {
    spi_dma_disable_rx();
    spi_set_2line_fullduplex();
    spi_cs_high(self->cs);
}
void is25x_write_no_dma(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {
    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_PP);
    spi_transfar_24be_no_cs(addr);

    /* spi_set_1line_txonly(); */
    for (size_t i = 0; i < len; i++) {
        spi_transfar_8_no_cs(src[i]);
        /* spi_write_8_no_rx(src[i]); */
    }
    /* spi_set_2line_fullduplex(); */

    spi_cs_high(self->cs);
}
void is25x_chip_erase(IS25x *self) { spi_transfar_8_with_cs(self->cs, IS25X_CER); }
void is25x_write_enable(IS25x *self) { spi_transfar_8_with_cs(self->cs, IS25X_WREN); }
uint8_t is25x_read_status_register(IS25x *self) {
    uint8_t result;
    spi_cs_low(self->cs);
    spi_transfar_8_no_cs(IS25X_RDSR);
    result = spi_transfar_8_no_cs(0);
    spi_cs_high(self->cs);
    return result;
}

// DataSheet p.64
IS25x_JEDEC_ProductID is25x_read_jedec_product_identification(IS25x *self) {
    IS25x_JEDEC_ProductID result;

    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_RDJDID);

    result.manufacturer_id = spi_transfar_8_no_cs(0);
    result.memory_type     = spi_transfar_8_no_cs(0);
    result.capacity        = spi_transfar_8_no_cs(0);

    spi_cs_high(self->cs);
    return result;
}
void is25x_enable_reset(IS25x *self) { spi_transfar_8_with_cs(self->cs, IS25X_RSTEN); }
void is25x_reset(IS25x *self) { spi_transfar_8_with_cs(self->cs, IS25X_RST); }
void is25x_software_reset(IS25x *self) {
    is25x_enable_reset(self);
    is25x_reset(self);
}
void is25x_erase_information_row(IS25x *self, uint32_t addr) {}
void is25x_write_information_row(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {}
void is25x_read_information_row(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {}

bool is25x_is_busy(IS25x *self) { return is25x_read_status_register(self) & IS25X_SREG_WIP; }
