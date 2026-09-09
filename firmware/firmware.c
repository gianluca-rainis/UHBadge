#include "firmware.h"
#include <stdlib.h>

/* 
    This function is runned only once at the start of the program.
*/
void setup() {
    // Initialize the GPIO pins
    gpio_init(GPIO_SD_CLK);
    gpio_set_dir(GPIO_SD_CLK, GPIO_OUT);
    gpio_init(GPIO_SD_CMD);
    gpio_set_dir(GPIO_SD_CMD, GPIO_OUT);

    gpio_init(GPIO_UP);
    gpio_set_dir(GPIO_UP, GPIO_IN);
    gpio_pull_up(GPIO_UP);
    gpio_init(GPIO_DOWN);
    gpio_set_dir(GPIO_DOWN, GPIO_IN);
    gpio_pull_up(GPIO_DOWN);
    gpio_init(GPIO_LEFT);
    gpio_set_dir(GPIO_LEFT, GPIO_IN);
    gpio_pull_up(GPIO_LEFT);
    gpio_init(GPIO_RIGHT);
    gpio_set_dir(GPIO_RIGHT, GPIO_IN);
    gpio_pull_up(GPIO_RIGHT);

    gpio_init(GPIO_NFC_SDA);
    gpio_set_dir(GPIO_NFC_SDA, GPIO_IN);
    gpio_init(GPIO_NFC_SCL);
    gpio_set_dir(GPIO_NFC_SCL, GPIO_IN);
    gpio_init(GPIO_NFC_GPO);
    gpio_set_dir(GPIO_NFC_GPO, GPIO_IN);

    gpio_init(GPIO_E_INK_BUSY);
    gpio_set_dir(GPIO_E_INK_BUSY, GPIO_IN);
    gpio_init(GPIO_E_INK_RST);
    gpio_set_dir(GPIO_E_INK_RST, GPIO_OUT);
    gpio_init(GPIO_E_INK_DC);
    gpio_set_dir(GPIO_E_INK_DC, GPIO_OUT);
    gpio_init(GPIO_E_INK_CS);
    gpio_set_dir(GPIO_E_INK_CS, GPIO_OUT);
    gpio_init(GPIO_E_INK_SCL);
    gpio_set_dir(GPIO_E_INK_SCL, GPIO_OUT);
    gpio_init(GPIO_E_INK_SDA);
    gpio_set_dir(GPIO_E_INK_SDA, GPIO_OUT);

    gpio_init(GPIO_SD_DAT0);
    gpio_set_dir(GPIO_SD_DAT0, GPIO_IN);
    gpio_init(GPIO_SD_DAT1);
    gpio_set_dir(GPIO_SD_DAT1, GPIO_IN);
    gpio_init(GPIO_SD_DAT2);
    gpio_set_dir(GPIO_SD_DAT2, GPIO_IN);
    gpio_init(GPIO_SD_DAT3);
    gpio_set_dir(GPIO_SD_DAT3, GPIO_IN);

    gpio_init(GPIO_PI_RADIO_MODULE_WLON_BTON);
    gpio_set_dir(GPIO_PI_RADIO_MODULE_WLON_BTON, GPIO_OUT);
    gpio_init(GPIO_PI_RADIO_MODULE_DI_DO_NIRQ);
    gpio_set_dir(GPIO_PI_RADIO_MODULE_DI_DO_NIRQ, GPIO_IN);
    gpio_init(GPIO_PI_RADIO_MODULE_CS);
    gpio_set_dir(GPIO_PI_RADIO_MODULE_CS, GPIO_OUT);

    gpio_init(GPIO_PI_RADIO_MODULE_SCLK);
    gpio_set_dir(GPIO_PI_RADIO_MODULE_SCLK, GPIO_OUT);

    // Expansion pins - decomment for use. Should be commented if not used.
    /* 
        * gpio_init(GPIO_EXPANSION_1);
        * gpio_set_dir(GPIO_EXPANSION_1, GPIO_OUT);
        * gpio_init(GPIO_EXPANSION_2);
        * gpio_set_dir(GPIO_EXPANSION_2, GPIO_OUT);
        * gpio_init(GPIO_EXPANSION_3);
        * gpio_set_dir(GPIO_EXPANSION_3, GPIO_OUT);
        * gpio_init(GPIO_EXPANSION_4);
        * gpio_set_dir(GPIO_EXPANSION_4, GPIO_OUT);
        * gpio_init(GPIO_EXPANSION_ADC0);
        * gpio_set_dir(GPIO_EXPANSION_ADC0, GPIO_IN);
        * gpio_init(GPIO_EXPANSION_ADC1);
        * gpio_set_dir(GPIO_EXPANSION_ADC1, GPIO_IN);
    */

    // Unconnected pins - decomment for nothing. Should be commented if not used (always, you can't use it).
    /* 
        * gpio_init(GPIO_UNCONNECTED_ADC2);
        * gpio_set_dir(GPIO_UNCONNECTED_ADC2, GPIO_IN);
    */
}

/* 
    This function is runned in a loop after the setup function.
*/
void loop() {

}