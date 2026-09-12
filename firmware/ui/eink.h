#include "firmware.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef UHBADGE_EINK
    #define UHBADGE_EINK

    #define EINK_HEIGHT  296
    #define EINK_WIDTH 128
    #define EINK_BUF_SIZE ((EINK_WIDTH * EINK_HEIGHT) / 8)

    #define EINK_SPI_PORT spi0
    #define EINK_SPI_BAUDRATE (4 * 1000 * 1000) // 4 MHz bandrate

    void einkInit();
    void einkClear();
    void einkWriteFramebuffer(const uint8_t* buf);
    void einkUpdate(bool partial);
    void einkSleep();
    void einkWaitBusy();
#endif
