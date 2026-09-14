#include "firmware.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef UHBADGE_NFC
    #define UHBADGE_NFC

    #define NFC_I2C_PORT i2c1
    #define NFC_I2C_BAUDRATE (100 * 1000) // 100 kHz
    #define NFC_ADDR_USER_MEMORY 0x53
    #define NFC_ADDR_SYSTEM_CONFIG 0x57
    #define NFC_MAX_URI_LEN 128

    void nfcInit();
    bool nfcWriteUri(const char* uri);
    bool nfcClear();
    bool nfcFieldDetected();
#endif
