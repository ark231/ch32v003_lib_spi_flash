#include "IS25x.h"

#include "spi_dma.h"

#define NOP asm volatile("nop")

static inline void spi_cs_low(uint8_t cs) { funDigitalWrite(cs, FUN_LOW); }
static inline void spi_cs_high(uint8_t cs) { funDigitalWrite(cs, FUN_HIGH); }

static inline uint8_t spi_transfar_8_no_cs(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
    asm volatile("nop");
    SPI_wait_RX_available();
    return SPI_read_8();
}
void is25x_init(IS25x *self, uint8_t cs) {
    self->cs = cs;
    funPinMode(cs, FUN_OUTPUT);
}

void is25x_read(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {
    spi_dma_read_init(len, sizeof(uint8_t));
    spi_dma_read_single_shot(dst, len, DMA_Priority_VeryHigh);
}
void is25x_write(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {}
void is25x_chip_erase(IS25x *self) {}
void is25x_write_enable(IS25x *self) {}
uint8_t is25x_read_status_register(IS25x *self) {}
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
void is25x_erase_information_row(IS25x *self, uint32_t addr) {}
void is25x_write_information_row(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {}
void is25x_read_information_row(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {}
