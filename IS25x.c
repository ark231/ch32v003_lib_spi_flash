#include "IS25x.h"

#include "spi_dma.h"

static inline uint8_t spi_transfer_8_no_cs(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
    asm volatile("nop");
    SPI_wait_RX_available();
    return SPI_read_8();
}
void is25x_init(IS25x *self, uint8_t cs) { self->cs = cs; }

void is25x_read(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {
    spi_dma_read_init(len, sizeof(uint8_t));
    spi_dma_read_single_shot(dst, len, DMA_Priority_VeryHigh);
}
void is25x_write(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {}
void is25x_chip_erase(IS25x *self) {}
void is25x_write_enable(IS25x *self) {}
uint8_t is25x_read_status_register(IS25x *self) {}
IS25x_JEDEC_ProductID is25x_read_jedec_product_identification(IS25x *self) {
    IS25x_JEDEC_ProductID result;

    SPI_NSS_software_low();

    SPI_write_8(IS25X_RDJDID);
    SPI_wait_TX_complete();
    asm volatile("nop");

    SPI_wait_RX_available();
    result.manufacturer_id = SPI_read_8();
    asm volatile("nop");  // maybe unnecessary?

    SPI_wait_RX_available();
    result.memory_type = SPI_read_8();
    asm volatile("nop");

    SPI_wait_RX_available();
    result.capacity = SPI_read_8();

    SPI_NSS_software_high();
    return result;
}
void is25x_erase_information_row(IS25x *self, uint32_t addr) {}
void is25x_write_information_row(IS25x *self, uint32_t addr, uint8_t *src, size_t len) {}
void is25x_read_information_row(IS25x *self, uint32_t addr, uint8_t *dst, size_t len) {}
