#include "eink.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

// SSD1680 commands
#define CMD_DRIVER_OUTPUT_CTRL 0x01
#define CMD_DATA_ENTRY_MODE 0x11
#define CMD_SW_RESET 0x12
#define CMD_TEMP_SENSOR_CTRL 0x18
#define CMD_MASTER_ACTIVATION 0x20
#define CMD_DISPLAY_UPDATE_CTRL1 0x21
#define CMD_DISPLAY_UPDATE_CTRL2 0x22
#define CMD_WRITE_RAM_BW 0x24
#define CMD_WRITE_VCOM 0x2C
#define CMD_WRITE_LUT 0x32
#define CMD_SET_RAM_X_ADDR 0x44
#define CMD_SET_RAM_Y_ADDR 0x45
#define CMD_SET_RAM_X_COUNTER 0x4E
#define CMD_SET_RAM_Y_COUNTER 0x4F
#define CMD_SLEEP 0x10
#define CMD_BORDER_WAVEFORM 0x3C

// LUT for partial refresh for standard 2.9" SSD1680.
static const uint8_t LUT_PARTIAL[159] = {
    0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0F,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,
    0x0
};

static inline void einkCsLow() {
    gpio_put(GPIO_E_INK_CS, 0);
}

static inline void einkCsHigh() {
    gpio_put(GPIO_E_INK_CS, 1);
}

static inline void einkDcCmd() {
    gpio_put(GPIO_E_INK_DC, 0);
}

static inline void einkDcData() {
    gpio_put(GPIO_E_INK_DC, 1);
}

static void einkSendCmd(uint8_t cmd) {
    einkDcCmd();

    einkCsLow();

    spi_write_blocking(EINK_SPI_PORT, &cmd, 1);

    einkCsHigh();
}

static void einkSendData(uint8_t data) {
    einkDcData();

    einkCsLow();

    spi_write_blocking(EINK_SPI_PORT, &data, 1);

    einkCsHigh();
}

static void einkSendDataBuf(const uint8_t* buf, size_t len) {
    einkDcData();

    einkCsLow();

    spi_write_blocking(EINK_SPI_PORT, buf, len);

    einkCsHigh();
}

void einkWaitBusy() {
    while (gpio_get(GPIO_E_INK_BUSY)) {
        sleep_ms(5);
    }
}

static void einkReset() {
    gpio_put(GPIO_E_INK_RST, 0);
    sleep_ms(10);

    gpio_put(GPIO_E_INK_RST, 1);
    sleep_ms(10);

    einkWaitBusy();
}

static void einkSetWindow(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end) {
    einkSendCmd(CMD_SET_RAM_X_ADDR);
    einkSendData((x_start >> 3) & 0xFF);
    einkSendData((x_end >> 3) & 0xFF);

    einkSendCmd(CMD_SET_RAM_Y_ADDR);
    einkSendData(y_start & 0xFF);
    einkSendData((y_start >> 8) & 0xFF);
    einkSendData(y_end & 0xFF);
    einkSendData((y_end >> 8) & 0xFF);
}

static void einkSetCursor(uint16_t x, uint16_t y) {
    einkSendCmd(CMD_SET_RAM_X_COUNTER);
    einkSendData((x >> 3) & 0xFF);

    einkSendCmd(CMD_SET_RAM_Y_COUNTER);
    einkSendData(y & 0xFF);
    einkSendData((y >> 8) & 0xFF);
}

static void einkLoadLut(const uint8_t* lut, size_t len) {
    einkSendCmd(CMD_WRITE_LUT);
    einkDcData();

    einkCsLow();

    spi_write_blocking(EINK_SPI_PORT, lut, len);

    einkCsHigh();
}

void einkInit() {
    spi_init(EINK_SPI_PORT, EINK_SPI_BAUDRATE);
    gpio_set_function(GPIO_E_INK_SCL, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_E_INK_SDA, GPIO_FUNC_SPI);

    gpio_init(GPIO_E_INK_CS);
    gpio_set_dir(GPIO_E_INK_CS, GPIO_OUT);
    gpio_put(GPIO_E_INK_CS, 1);

    gpio_init(GPIO_E_INK_DC);
    gpio_set_dir(GPIO_E_INK_DC, GPIO_OUT);

    gpio_init(GPIO_E_INK_RST);
    gpio_set_dir(GPIO_E_INK_RST, GPIO_OUT);

    gpio_init(GPIO_E_INK_BUSY);
    gpio_set_dir(GPIO_E_INK_BUSY, GPIO_IN);

    einkReset();

    einkSendCmd(CMD_SW_RESET);
    einkWaitBusy();

    einkSendCmd(CMD_DRIVER_OUTPUT_CTRL);
    einkSendData((EINK_HEIGHT - 1) & 0xFF);
    einkSendData(((EINK_HEIGHT - 1) >> 8) & 0xFF);
    einkSendData(0x00);

    einkSendCmd(CMD_DATA_ENTRY_MODE);
    einkSendData(0x03);

    einkSetWindow(0, EINK_WIDTH - 1, 0, EINK_HEIGHT - 1);

    einkSendCmd(CMD_BORDER_WAVEFORM);
    einkSendData(0x05);

    einkSendCmd(CMD_TEMP_SENSOR_CTRL);
    einkSendData(0x80);

    einkSetCursor(0, 0);
    einkWaitBusy();
}

void einkWriteFramebuffer(const uint8_t* buf) {
    einkSetWindow(0, EINK_WIDTH - 1, 0, EINK_HEIGHT - 1);
    einkSetCursor(0, 0);

    einkSendCmd(CMD_WRITE_RAM_BW);
    einkSendDataBuf(buf, EINK_BUF_SIZE);
}

void einkUpdate(bool partial) {
    if (partial) {
        einkLoadLut(LUT_PARTIAL, sizeof(LUT_PARTIAL));
        einkSendCmd(CMD_DISPLAY_UPDATE_CTRL2);
        einkSendData(0xFF);
    }
    else {
        einkSendCmd(CMD_DISPLAY_UPDATE_CTRL2);
        einkSendData(0xF7);
    }

    einkSendCmd(CMD_MASTER_ACTIVATION);
    einkWaitBusy();
}

void einkClear() {
    uint8_t blank[EINK_BUF_SIZE];

    for (size_t i = 0; i < EINK_BUF_SIZE; i++) {
        blank[i] = 0xFF;
    }

    einkWriteFramebuffer(blank);
    einkUpdate(false);
}

void einkSleep() {
    einkSendCmd(CMD_SLEEP);
    einkSendData(0x01);
}
