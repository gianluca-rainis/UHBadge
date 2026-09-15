#include "firmware.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef UHBADGE_SD
    #define UHBADGE_SD

    #define SD_SPI_PORT spi0
    #define SD_SPI_CS_PIN GPIO_SD_DAT3

    #define SD_BLOCK_SIZE 512

    bool sdSpiInit();
    bool sdSpiIsBlockAddressed();
    bool sdSpiReadBlock(uint32_t lba, uint8_t* buf);
    bool sdSpiWriteBlock(uint32_t lba, const uint8_t* buf);
#endif
