#include "sd.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define CMD0 0 // GO_IDLE_STATE
#define CMD8 8 // SEND_IF_COND
#define CMD17 17 // READ_SINGLE_BLOCK
#define CMD24 24 // WRITE_BLOCK
#define CMD55 55 // APP_CMD (before ACMD41)
#define CMD58 58 // READ_OCR
#define ACMD41 41 // SD_SEND_OP_COND (after CMD55)

static bool blockAddressed = false;

static inline void sdCsLow() {
    gpio_put(SD_SPI_CS_PIN, 0);
}

static inline void sdCsHigh() {
    gpio_put(SD_SPI_CS_PIN, 1);
}

static uint8_t sdSpiXfer(uint8_t out) {
    uint8_t in;

    spi_write_read_blocking(SD_SPI_PORT, &out, &in, 1);

    return in;
}

static void sdSpiIdleClocks(int n) {
    for (int i = 0; i < n; i++) {
        sdSpiXfer(0xFF);
    }
}

static uint8_t sdSendCmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    sdSpiXfer(0x40 | cmd);
    sdSpiXfer((arg >> 24) & 0xFF);
    sdSpiXfer((arg >> 16) & 0xFF);
    sdSpiXfer((arg >> 8) & 0xFF);
    sdSpiXfer(arg & 0xFF);
    sdSpiXfer(crc);

    uint8_t response = 0xFF;

    for (int i = 0; i < 10; i++) {
        response = sdSpiXfer(0xFF);

        if ((response & 0x80) == 0) { // bit 7 = 0
            break;
        }
    }

    return response;
}

static uint8_t sdSendAcmd(uint8_t acmd, uint32_t arg) {
    sdSendCmd(CMD55, 0, 0x01); // APP_CMD before ACMD

    return sdSendCmd(acmd, arg, 0x01);
}

bool sdSpiInit() {
    spi_init(SD_SPI_PORT, 400 * 1000); // 400 kHz
    gpio_set_function(GPIO_SD_CLK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_SD_CMD, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_SD_DAT0, GPIO_FUNC_SPI);

    gpio_init(SD_SPI_CS_PIN);
    gpio_set_dir(SD_SPI_CS_PIN, GPIO_OUT);
    sdCsHigh();

    gpio_init(GPIO_SD_DAT1);
    gpio_set_dir(GPIO_SD_DAT1, GPIO_IN);
    gpio_pull_up(GPIO_SD_DAT1);
    gpio_init(GPIO_SD_DAT2);
    gpio_set_dir(GPIO_SD_DAT2, GPIO_IN);
    gpio_pull_up(GPIO_SD_DAT2);

    sdSpiIdleClocks(10);

    sdCsLow();

    uint8_t r1 = sdSendCmd(CMD0, 0, 0x95);

    if (r1 != 0x01) {
        sdCsHigh();

        return false;
    }

    r1 = sdSendCmd(CMD8, 0x000001AA, 0x87);

    bool isV2 = (r1 == 0x01);

    if (isV2) {
        sdSpiXfer(0xFF);
        sdSpiXfer(0xFF);
        sdSpiXfer(0xFF);
        sdSpiXfer(0xFF);
    }

    uint32_t acmdArg = isV2 ? (1UL << 30) : 0;
    int attempts = 0;

    do {
        r1 = sdSendAcmd(ACMD41, acmdArg);

        sleep_ms(2);

        attempts++;
    } while (r1 != 0x00 && attempts < 1000);

    if (r1 != 0x00) {
        sdCsHigh();

        return false;
    }

    blockAddressed = false;

    if (isV2) {
        r1 = sdSendCmd(CMD58, 0, 0x01);

        if (r1 == 0x00) {
            uint8_t ocr0 = sdSpiXfer(0xFF);

            sdSpiXfer(0xFF);
            sdSpiXfer(0xFF);
            sdSpiXfer(0xFF);

            blockAddressed = (ocr0 & 0x40) != 0; // bit 30 ocr0 = CCS (Card Capacity Status)
        }
    }

    sdCsHigh();
    sdSpiIdleClocks(1);

    spi_set_baudrate(SD_SPI_PORT, 20 * 1000 * 1000); // 20 MHz

    return true;
}

bool sdSpiIsBlockAddressed() {
    return blockAddressed;
}

bool sdSpiReadBlock(uint32_t lba, uint8_t* buf) {
    uint32_t addr = blockAddressed ? lba : lba * SD_BLOCK_SIZE;

    sdCsLow();

    uint8_t r1 = sdSendCmd(CMD17, addr, 0x01);

    if (r1 != 0x00) {
        sdCsHigh();

        return false;
    }

    uint8_t token = 0xFF;

    for (int i = 0; i < 8000 && token != 0xFE; i++) {
        token = sdSpiXfer(0xFF);
    }

    if (token != 0xFE) {
        sdCsHigh();

        return false;
    }

    for (int i = 0; i < SD_BLOCK_SIZE; i++) {
        buf[i] = sdSpiXfer(0xFF);
    }

    sdSpiXfer(0xFF);
    sdSpiXfer(0xFF);

    sdCsHigh();
    sdSpiIdleClocks(1);

    return true;
}

bool sdSpiWriteBlock(uint32_t lba, const uint8_t* buf) {
    uint32_t addr = blockAddressed ? lba : lba * SD_BLOCK_SIZE;

    sdCsLow();

    uint8_t r1 = sdSendCmd(CMD24, addr, 0x01);

    if (r1 != 0x00) {
        sdCsHigh();

        return false;
    }

    sdSpiXfer(0xFE);

    for (int i = 0; i < SD_BLOCK_SIZE; i++) {
        sdSpiXfer(buf[i]);
    }

    sdSpiXfer(0xFF);
    sdSpiXfer(0xFF);

    uint8_t dataResponse = sdSpiXfer(0xFF);

    if ((dataResponse & 0x1F) != 0x05) { // 0bxxx0101
        sdCsHigh();

        return false;
    }

    int timeout = 100000;

    while (sdSpiXfer(0xFF) == 0x00 && timeout-- > 0) {
        tight_loop_contents();
    }

    sdCsHigh();
    sdSpiIdleClocks(1);
    
    return timeout > 0;
}
