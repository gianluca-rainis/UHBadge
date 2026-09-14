#include "nfc.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <string.h>

#define NFC_USER_MEM_START 0x0000

static void buildCapabilityContainer(uint8_t* cc) {
    cc[0] = 0xE2;
    cc[1] = 0x40;
    cc[2] = 0x00;
    cc[3] = 0x01;
    cc[4] = 0x00;
    cc[5] = 0x00;
    cc[6] = 0x04;
    cc[7] = 0x00;
}

static bool i2cWriteMem(uint8_t devAddr, uint16_t memAddr, const uint8_t* data, size_t len) {
    uint8_t buf[2 + 16];
    size_t offset = 0;

    while (offset < len) {
        size_t chunk = len - offset;

        if (chunk > 16) {
            chunk = 16;
        }

        buf[0] = (memAddr >> 8) & 0xFF;
        buf[1] = memAddr & 0xFF;
        memcpy(&buf[2], data + offset, chunk);

        int result = i2c_write_blocking(NFC_I2C_PORT, devAddr, buf, chunk + 2, false);

        if (result < 0) {
            return false;
        }

        sleep_ms(6);

        memAddr += chunk;
        offset += chunk;
    }

    return true;
}

static uint8_t uriAbbreviationCode(const char* uri, size_t* prefixLen) {
    struct { const char* prefix; uint8_t code; } table[] = {
        { "http://www.", 0x01 },
        { "https://www.", 0x02 },
        { "http://", 0x03 },
        { "https://", 0x04 },
    };

    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        size_t len = strlen(table[i].prefix);

        if (strncmp(uri, table[i].prefix, len) == 0) {
            *prefixLen = len;

            return table[i].code;
        }
    }

    *prefixLen = 0;

    return 0x00;
}

void nfcInit() {
    i2c_init(NFC_I2C_PORT, NFC_I2C_BAUDRATE);
    gpio_set_function(GPIO_NFC_SDA, GPIO_FUNC_I2C);
    gpio_set_function(GPIO_NFC_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(GPIO_NFC_SDA);
    gpio_pull_up(GPIO_NFC_SCL);

    gpio_init(GPIO_NFC_GPO);
    gpio_set_dir(GPIO_NFC_GPO, GPIO_IN);
}

bool nfcWriteUri(const char* uri) {
    size_t uriLen = strlen(uri);

    if (uriLen == 0 || uriLen > NFC_MAX_URI_LEN) {
        return false;
    }

    size_t prefixLen;
    uint8_t idCode = uriAbbreviationCode(uri, &prefixLen);
    const char* payloadUri = uri + prefixLen;
    size_t payloadUriLen = uriLen - prefixLen;

    uint8_t payloadLen = 1 + payloadUriLen;
    uint8_t record[4 + 1 + NFC_MAX_URI_LEN];
    size_t idx = 0;

    record[idx++] = 0xD1; // MB=1, ME=1, SR=1, TNF=0x01 (well-known type)
    record[idx++] = 0x01; // Type length
    record[idx++] = payloadLen;
    record[idx++] = 'U';
    record[idx++] = idCode;
    memcpy(&record[idx], payloadUri, payloadUriLen);
    idx += payloadUriLen;

    size_t recordLen = idx;

    uint8_t full[8 + 4 + sizeof(record) + 1];
    size_t fullIdx = 0;

    buildCapabilityContainer(&full[fullIdx]);
    fullIdx += 8;

    full[fullIdx++] = 0x03;

    if (recordLen < 0xFF) {
        full[fullIdx++] = (uint8_t)recordLen;
    }
    else {
        full[fullIdx++] = 0xFF;
        full[fullIdx++] = (recordLen >> 8) & 0xFF;
        full[fullIdx++] = recordLen & 0xFF;
    }

    memcpy(&full[fullIdx], record, recordLen);
    fullIdx += recordLen;

    full[fullIdx++] = 0xFE;

    return i2cWriteMem(NFC_ADDR_USER_MEMORY, NFC_USER_MEM_START, full, fullIdx);
}

bool nfcClear() {
    uint8_t empty[8 + 3];

    buildCapabilityContainer(empty);
    empty[8] = 0x03;  // NDEF Message TLV
    empty[9] = 0x00;  // Length (0 is empty message)
    empty[10] = 0xFE; // Terminator TLV

    return i2cWriteMem(NFC_ADDR_USER_MEMORY, NFC_USER_MEM_START, empty, sizeof(empty));
}

bool nfcFieldDetected() {
    return gpio_get(GPIO_NFC_GPO);
}
