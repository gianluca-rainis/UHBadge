#include "tools/buttons.h"
#include "framebuffer.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef UHBADGE_UI
    #define UHBADGE_UI

    #define MENU_ROWS 4
    #define MENU_COLS 2
    #define MENU_VISIBLE (MENU_ROWS * MENU_COLS)

    #define BADGE_IMAGE_DIAMETER 48
    #define BADGE_IMAGE_BUF_SIZE ((BADGE_IMAGE_DIAMETER * ((BADGE_IMAGE_DIAMETER + 7) / 8)) + 2)

    typedef struct {
        const char* name;
        const uint8_t* icon;

        void (*open)();
    } App;

    typedef enum {
        SCREEN_BADGE,               // Main badge screen
        SCREEN_CONTACT,             // Contact information screen
        SCREEN_SETTINGS,            // Settings screen
        SCREEN_GITHUB,              // GitHub profile screen - powered by GitHub API
        SCREEN_GITHUB_STATS,        // GitHub statistics screen - powered by GitHub API
        SCREEN_GITHUB_TROPHIES,     // GitHub trophies screen - powered by GitHub API
        SCREEN_APPS_MENU            // Applications menu screen
    } ScreenId;

    void uiInit();
    void uiUpdate();
    void uiGoToScreen(ScreenId screen);
#endif
