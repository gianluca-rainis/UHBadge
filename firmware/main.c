/* 
    * Pinout:
    * 
    * GPIO0 - Expansion Pin
    * GPIO1 - Expansion Pin
    * GPIO2 - Expansion Pin
    * GPIO3 - Expansion Pin
    * GPIO4 - SD CLK
    * GPIO5 - SD CMD
    * GPIO6 - UP
    * GPIO7 - DOWN
    * GPIO8 - LEFT
    * GPIO9 - RIGHT
    * GPIO10 - NFC SDA
    * GPIO11 - NFC SCL
    * GPIO12 - NFC GPO
    * GPIO13 - E-INK BUSY
    * GPIO14 - E-INK RST
    * GPIO15 - E-INK D/C
    * GPIO16 - E-INK CS
    * GPIO17 - E-INK SCL
    * GPIO18 - E-INK SDA
    * GPIO19 - SD DAT0
    * GPIO20 - SD DAT1
    * GPIO21 - SD DAT2
    * GPIO22 - SD DAT3
    * GPIO23 - Pi Radio Module WL_ON + BT_ON
    * GPIO24 - Pi Radio Module DI + DO + nIRQ
    * GPIO25 - Pi Radio Module CS
    * GPIO26/ADC0 - Expansion Pin
    * GPIO27/ADC1 - Expansion Pin
    * GPIO28/ADC2 - UNCONNECTED
    * GPIO29/ADC3 - Pi Radio Module SCLK
*/

#include "firmware.h"
#include <stdbool.h>

int main() {
    setup();

    while (true)
    {
        loop();
    }

    return 0;
}
