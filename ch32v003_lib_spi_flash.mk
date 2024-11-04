ch32v003_lib_spi_flash?=./
ADDITIONAL_C_FILES+=$(ch32v003_lib_spi_flash)/spi_dma.c $(ch32v003_lib_spi_flash)/IS25x.c
EXTRA_CFLAGS+=-I$(ch32v003_lib_spi_flash)
