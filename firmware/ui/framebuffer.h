#include "eink.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef UHBADGE_FRAMEBUFFER
    #define UHBADGE_FRAMEBUFFER

    #define FB_WHITE 0
    #define FB_BLACK 1
    #define FB_FONT_WIDTH  5
    #define FB_FONT_HEIGHT 7

    void fbClear();

    const uint8_t* fbGetBuffer();

    void fbSetPixel(int x, int y, int color);
    int fbGetPixel(int x, int y);

    void fbDrawHline(int x, int y, int w, int color);
    void fbDrawVline(int x, int y, int h, int color);
    void fbDrawRect(int x, int y, int w, int h, bool filled, int color);
    int fbDrawChar(int x, int y, char c, int color);
    void fbDrawText(int x, int y, const char* text, int color, int wrap_width);
    void fbDrawBitmap(int x, int y, int w, int h, const uint8_t* bitmap, int color);
    void fbDrawBitmapCircular(int cx, int cy, int diameter, const uint8_t* bitmap, int color);
#endif
