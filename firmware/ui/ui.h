#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "tools/buttons.h"

#ifndef UHBADGE_UI
    #define UHBADGE_UI

    #define MENU_ROWS 4
    #define MENU_COLS 2
    #define MENU_VISIBLE (MENU_ROWS * MENU_COLS)

    typedef struct {
        const char* name;
        const uint8_t* icon;

        void (*open)();
    } App;

    void ui_init();
    void ui_draw();
#endif
