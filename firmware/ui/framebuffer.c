#include "framebuffer.h"
#include "font.h"

#include <string.h>
#include <ctype.h>

#define ROW_BYTES (EINK_WIDTH / 8)

static uint8_t buffer[EINK_BUF_SIZE];

void fbClear() {
    memset(buffer, 0xFF, EINK_BUF_SIZE);
}

const uint8_t* fbGetBuffer() {
    return buffer;
}

void fbSetPixel(int x, int y, int color) {
    if (x < 0 || x >= EINK_WIDTH || y < 0 || y >= EINK_HEIGHT) {
        return;
    }

    int byteIndex = y * ROW_BYTES + (x / 8);
    uint8_t mask = 0x80 >> (x % 8);

    if (color == FB_BLACK) {
        buffer[byteIndex] &= ~mask;
    }
    else {
        buffer[byteIndex] |= mask;
    }
}

int fbGetPixel(int x, int y) {
    if (x < 0 || x >= EINK_WIDTH || y < 0 || y >= EINK_HEIGHT) {
        return FB_WHITE;
    }

    int byteIndex = y * ROW_BYTES + (x / 8);
    uint8_t mask = 0x80 >> (x % 8);

    return (buffer[byteIndex] & mask) ? FB_WHITE : FB_BLACK;
}

void fbDrawHline(int x, int y, int w, int color) {
    for (int i = 0; i < w; i++) {
        fbSetPixel(x + i, y, color);
    }
}

void fbDrawVline(int x, int y, int h, int color) {
    for (int i = 0; i < h; i++) {
        fbSetPixel(x, y + i, color);
    }
}

void fbDrawRect(int x, int y, int w, int h, bool filled, int color) {
    if (filled) {
        for (int row = 0; row < h; row++) {
            fbDrawHline(x, y + row, w, color);
        }
    }
    else {
        fbDrawHline(x, y, w, color);
        fbDrawHline(x, y + h - 1, w, color);
        fbDrawVline(x, y, h, color);
        fbDrawVline(x + w - 1, y, h, color);
    }
}

int fbDrawChar(int x, int y, char c, int color) {
    char ch = toupper((unsigned char)c);

    if (ch < FONT_FIRST_CHAR || ch > FONT_LAST_CHAR) { // Not supported character
        ch = ' ';
    }

    const uint8_t* glyph = FONT[ch - FONT_FIRST_CHAR];

    for (int col = 0; col < FB_FONT_WIDTH; col++) {
        uint8_t colBits = glyph[col];

        for (int row = 0; row < FB_FONT_HEIGHT; row++) {
            if (colBits & (1 << row)) {
                fbSetPixel(x + col, y + row, color);
            }
        }
    }

    return FB_FONT_WIDTH + 1; // Space between characters
}

void fbDrawText(int x, int y, const char* text, int color, int wrap_width) {
    int cursorX = x;
    int cursorY = y;
    int startX = x;

    for (const char* p = text; *p != '\0'; p++) {
        if (*p == '\n') {
            cursorX = startX;
            cursorY += FB_FONT_HEIGHT + 2;

            continue;
        }

        int advance = fbDrawChar(cursorX, cursorY, *p, color);
        cursorX += advance;

        if (wrap_width > 0 && (cursorX - startX) >= wrap_width) {
            cursorX = startX;
            cursorY += FB_FONT_HEIGHT + 2;
        }
    }
}

void fbDrawBitmap(int x, int y, int w, int h, const uint8_t* bitmap, int color) {
    int rowBytes = (w + 7) / 8;

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int byteIndex = row * rowBytes + (col / 8);
            uint8_t mask = 0x80 >> (col % 8);

            if (bitmap[byteIndex] & mask) {
                fbSetPixel(x + col, y + row, color);
            }
        }
    }
}

void fbDrawBitmapCircular(int cx, int cy, int diameter, const uint8_t* bitmap, int color) {
    int radius = diameter / 2;
    int rowBytes = (diameter + 7) / 8;

    for (int row = 0; row < diameter; row++) {
        for (int col = 0; col < diameter; col++) {
            int dx = col - radius;
            int dy = row - radius;

            if (dx * dx + dy * dy > radius * radius) {
                continue;
            }

            int byteIndex = row * rowBytes + (col / 8);
            uint8_t mask = 0x80 >> (col % 8);

            if (bitmap[byteIndex] & mask) {
                fbSetPixel(cx - radius + col, cy - radius + row, color);
            }
        }
    }
}
