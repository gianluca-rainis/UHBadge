#include "qrRender.h"
#include "../ui/framebuffer.h"
#include "../ui/eink.h"
#include "../lib/qrCodeGen/qrcodegen.h"

#define QR_MAX_VERSION 10
#define QR_MIN_SCALE 2

int qrRenderDraw(const char* text, int topY) {
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)];
    uint8_t qrcode[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)];

    bool ok = qrcodegen_encodeText(
        text, tempBuffer, qrcode,
        qrcodegen_Ecc_LOW,
        qrcodegen_VERSION_MIN, QR_MAX_VERSION,
        qrcodegen_Mask_AUTO, true
    );

    if (!ok) {
        return 0;
    }

    int size = qrcodegen_getSize(qrcode);

    int scale = (EINK_WIDTH - 8) / size;
    
    if (scale < 1) {
        scale = 1;
    }

    int totalPx = size * scale;
    int startX = (EINK_WIDTH - totalPx) / 2;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcodegen_getModule(qrcode, x, y)) {
                fbDrawRect(startX + x * scale, topY + y * scale, scale, scale, true, FB_BLACK);
            }
        }
    }

    return totalPx;
}
