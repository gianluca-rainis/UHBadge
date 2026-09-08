#include <stdio.h>
#include <stdbool.h>

#ifndef UHBADGE_FIRMWARE
    #define UHBADGE_FIRMWARE

    #define GPIO_EXPANSION_1 0
    #define GPIO_EXPANSION_2 1
    #define GPIO_EXPANSION_3 2
    #define GPIO_EXPANSION_4 3

    #define GPIO_SD_CLK 4
    #define GPIO_SD_CMD 5

    #define GPIO_UP 6
    #define GPIO_DOWN 7
    #define GPIO_LEFT 8
    #define GPIO_RIGHT 9

    #define GPIO_NFC_SDA 10
    #define GPIO_NFC_SCL 11
    #define GPIO_NFC_GPO 12

    #define GPIO_E_INK_BUSY 13
    #define GPIO_E_INK_RST 14
    #define GPIO_E_INK_DC 15
    #define GPIO_E_INK_CS 16
    #define GPIO_E_INK_SCL 17
    #define GPIO_E_INK_SDA 18

    #define GPIO_SD_DAT0 19
    #define GPIO_SD_DAT1 20
    #define GPIO_SD_DAT2 21
    #define GPIO_SD_DAT3 22

    #define GPIO_PI_RADIO_MODULE_WLON_BTON 23
    #define GPIO_PI_RADIO_MODULE_DI_DO_NIRQ 24
    #define GPIO_PI_RADIO_MODULE_CS 25

    #define GPIO_EXPANSION_ADC0 26
    #define GPIO_EXPANSION_ADC1 27
    
    #define GPIO_UNCONNECTED_ADC2 28

    #define GPIO_PI_RADIO_MODULE_SCLK 29

    // Main functions
    void setup();
    void loop();
#endif

#ifndef null
    #define null 0
#endif
