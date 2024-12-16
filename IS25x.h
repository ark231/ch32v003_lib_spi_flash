#ifndef CH32V003_LIB_SPI_FLASH_IS25X
#define CH32V003_LIB_SPI_FLASH_IS25X
// clang-format off
#include "funconfig.h" // NOLINT(unused-includes)
#include <ch32v003_SPI.h>
// clang-format on
#include <stdint.h>

#include "spi_dma.h"

typedef struct {
    uint8_t cs;
} IS25x;

#define IS25X_NORD   0x03
#define IS25X_PP     0x02
#define IS25X_CER    0xC7
#define IS25X_WREN   0x06
#define IS25X_RDSR   0x05
#define IS25X_RDJDID 0x9F
#define IS25X_RSTEN  0x66
#define IS25X_RST    0x99
#define IS25X_IRER   0x64
#define IS25X_IRP    0x62
#define IS25X_IRRD   0x68

#define IS25X_SREG_WIP 1  // 1 on busy, 0 on ready

typedef struct {
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t capacity;
} IS25x_JEDEC_ProductID;

// initialize is25x
void is25x_init(IS25x *self, uint8_t cs);

// read from is25x without dma, blocking until finished
void is25x_read_no_dma(IS25x *self, uint32_t addr, uint8_t *dst, size_t len);

// nonblocking read operation using dma
void is25x_begin_dma_read(IS25x *self, uint32_t addr, uint8_t *dst, size_t len);
bool is25x_dma_read_is_completed(IS25x *self);
void is25x_end_dma_read(volatile IS25x *self);  // private, do not call from user code

void is25x_write_no_dma(IS25x *self, uint32_t addr, uint8_t *src, size_t len);

void is25x_chip_erase(IS25x *self);
void is25x_write_enable(IS25x *self);
uint8_t is25x_read_status_register(IS25x *self);
IS25x_JEDEC_ProductID is25x_read_jedec_product_identification(IS25x *self);
void is25x_enable_reset(IS25x *self);
void is25x_reset(IS25x *self);
void is25x_software_reset(IS25x *self);
void is25x_erase_information_row(IS25x *self, uint32_t addr);
void is25x_write_information_row(IS25x *self, uint32_t addr, uint8_t *src, size_t len);
void is25x_read_information_row(IS25x *self, uint32_t addr, uint8_t *dst, size_t len);

bool is25x_is_busy(IS25x *self);

#endif
