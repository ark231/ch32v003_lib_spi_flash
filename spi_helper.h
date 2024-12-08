#ifndef CH32V003_LIB_SPI_FLASH_SPI_HELPER
#define CH32V003_LIB_SPI_FLASH_SPI_HELPER
#include <ch32v003_SPI.h>
#include <ch32v003fun.h>
#include <stdint.h>

static inline void spi_cs_low(uint8_t cs) { funDigitalWrite(cs, FUN_LOW); }
static inline void spi_cs_high(uint8_t cs) { funDigitalWrite(cs, FUN_HIGH); }

static inline uint8_t spi_transfar_8_no_cs(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
    __NOP();
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

static inline void spi_write_8_no_rx(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
}

static inline uint8_t spi_read_8_no_tx(void) {
    SPI_wait_RX_available();
    return SPI_read_8();
}

static inline void spi_write_24be_no_rx(uint32_t data) {
    spi_write_8_no_rx((data >> 16) & 0xff);
    spi_write_8_no_rx((data >> 8) & 0xff);
    spi_write_8_no_rx((data) & 0xff);
}

static inline uint8_t spi_transfar_8_with_cs(uint8_t cs, uint8_t data) {
    uint8_t result;
    spi_cs_low(cs);
    result = spi_transfar_8_no_cs(data);
    spi_cs_high(cs);
    return result;
}

static inline void spi_set_2line_rxonly() {
    SPI1->CTLR1 &= ~(SPI_CTLR1_BIDIMODE | SPI_CTLR1_BIDIOE | SPI_CTLR1_RXONLY);
    SPI1->CTLR1 |= SPI_Direction_2Lines_RxOnly;
}
static inline void spi_set_2line_fullduplex() {
    SPI1->CTLR1 &= ~(SPI_CTLR1_BIDIMODE | SPI_CTLR1_BIDIOE | SPI_CTLR1_RXONLY);
    SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;
}
static inline void spi_set_1line_rxonly() {
    SPI1->CTLR1 &= ~(SPI_CTLR1_BIDIMODE | SPI_CTLR1_BIDIOE | SPI_CTLR1_RXONLY);
    SPI1->CTLR1 |= SPI_Direction_1Line_Rx;
}
static inline void spi_set_1line_txonly() {
    SPI1->CTLR1 &= ~(SPI_CTLR1_BIDIMODE | SPI_CTLR1_BIDIOE | SPI_CTLR1_RXONLY);
    SPI1->CTLR1 |= SPI_Direction_1Line_Tx;
}
#endif
