#include "IS25x.h"

#include "spi_dma.h"

#define NOP __NOP

static inline void spi_cs_low(uint8_t cs) { funDigitalWrite(cs, FUN_LOW); }
static inline void spi_cs_high(uint8_t cs) { funDigitalWrite(cs, FUN_HIGH); }

static inline uint8_t spi_transfar_8_no_cs(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
    NOP();
    SPI_wait_RX_available();
    return SPI_read_8();
}

static inline uint32_t spi_transfar_24be_no_cs(uint32_t data) {
    uint32_t result = 0;
    result |= spi_transfar_8_no_cs((data >> 16) & 0xff) << 16;
    result |= spi_transfar_8_no_cs((data >> 8) & 0xff) << 8;
    result |= spi_transfar_8_no_cs((data) & 0xff);
    return result;
}

static inline uint8_t spi_transfar_8_with_cs(uint8_t cs, uint8_t data) {
    uint8_t result;
    spi_cs_low(cs);
    result = spi_transfar_8_no_cs(data);
    spi_cs_high(cs);
    return result;
}

void is25x_init(IS25x *self, uint8_t cs) {
    self->cs = cs;
    funPinMode(cs, FUN_OUTPUT);
}

void is25x_read_no_dma(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {
    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_NORD);
    spi_transfar_24be_no_cs(addr);
    for (size_t i = 0; i < len; i++) {
        dst[i] = spi_transfar_8_no_cs(0xff);  // HACK: writing 0 resulted in unstable behaviour, and writing 0xff solved
                                              // it. I TOTALLY DON'T KNOW WHY!!!
        NOP();
    }

    /* spi_dma_read_init(len, sizeof(uint8_t)); */
    /* spi_dma_read_single_shot(dst, len, DMA_Priority_VeryHigh); */

    spi_cs_high(self->cs);
}
void is25x_write_no_dma(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {
    spi_cs_low(self->cs);

    spi_transfar_8_no_cs(IS25X_PP);
    spi_transfar_24be_no_cs(addr);
    for (size_t i = 0; i < len; i++) {
        spi_transfar_8_no_cs(src[i]);
    }

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
