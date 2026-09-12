#include "ui.h"

int selected = 0;
int scroll = 0;
int menuTotalElements = 0;

void ui_init() {
    /* App apps[] = {
        {
            "WiFi",
            icon_wifi,
            open_wifi
        },
        {
            "NFC",
            icon_nfc,
            open_nfc
        },
        {
            "Settings",
            icon_settings,
            open_settings
        }
    };

    menuTotalElements = sizeof(apps) / sizeof(App);
     */
}

void ui_draw() {
    int selectedRow = selected / MENU_COLS;
    int selectedCol = selected % MENU_COLS;

    if (buttonUpPressed()) {
        if (selectedRow > 0) {
            selected -= MENU_COLS;
            selectedRow--;
        }

        if (selectedRow <= scroll * MENU_ROWS) {
            scroll--;
        }
    }

    if (buttonDownPressed()) {
        if (selectedRow < (menuTotalElements / MENU_COLS)) {
            selected += MENU_COLS;
            selectedRow++;
        }

        if (selectedRow >= (scroll + 1) * MENU_ROWS) {
            scroll++;
        }
    }
    
    if (buttonLeftPressed()) {
        if (selectedCol != 0) {
            selected--;
            selectedCol--;
        }
    }

    if (buttonRightPressed()) {
        if (selectedCol != MENU_COLS - 1 && selected < menuTotalElements - 1) {
            selected++;
            selectedCol++;
        }
    }
}
