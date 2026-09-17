#include "ui.h"
#include "eink.h"

#include <string.h>

static ScreenId currentScreen = SCREEN_BADGE;
static bool needsRedraw = true;
static bool firstFullRefreshDone = false;

static bool prevUp = false;
static bool prevDown = false;
static bool prevLeft = false;
static bool prevRight = false;

int selected = 0;
int scroll = 0;
int menuTotalElements = 0;

static void openWifi();
static void openBluetooth();
static void openNfc();
static void openSettings();

static App apps[] = {
    { "WiFi", NULL, openWifi },
    { "Bluetooth", NULL, openBluetooth },
    { "NFC", NULL, openNfc },
    { "Settings", NULL, openSettings },
};

static void openWifi() {
    uiGoToScreen(SCREEN_SETTINGS);
}

static void openBluetooth() {
    uiGoToScreen(SCREEN_SETTINGS);
}

static void openNfc() {
    uiGoToScreen(SCREEN_SETTINGS);
}

static void openSettings() {
    uiGoToScreen(SCREEN_SETTINGS);
}

void uiGoToScreen(ScreenId screen) {
    currentScreen = screen;
    needsRedraw = true;
}

static void readButtonEdges(bool* upEdge, bool* downEdge, bool* leftEdge, bool* rightEdge) {
    bool up = buttonUpPressed();
    bool down = buttonDownPressed();
    bool left = buttonLeftPressed();
    bool right = buttonRightPressed();

    *upEdge = up && !prevUp;
    *downEdge = down && !prevDown;
    *leftEdge = left && !prevLeft;
    *rightEdge = right && !prevRight;

    prevUp = up;
    prevDown = down;
    prevLeft = left;
    prevRight = right;
}

static void handleCrossNavigation(bool upEdge, bool downEdge, bool leftEdge, bool rightEdge) {
    switch (currentScreen) {
        case SCREEN_BADGE:
            if (upEdge) {
                uiGoToScreen(SCREEN_SETTINGS);
            }

            if (downEdge) {
                uiGoToScreen(SCREEN_CONTACT);
            }

            if (leftEdge) {
                uiGoToScreen(SCREEN_APPS_MENU);
            }

            if (rightEdge) {
                uiGoToScreen(SCREEN_GITHUB);
            }
            break;

        case SCREEN_CONTACT:
            if (upEdge) {
                uiGoToScreen(SCREEN_BADGE);
            }
            break;

        case SCREEN_SETTINGS:
            if (downEdge) {
                uiGoToScreen(SCREEN_BADGE);
            }
            break;

        case SCREEN_GITHUB:
            if (leftEdge) {
                uiGoToScreen(SCREEN_BADGE);
            }

            if (upEdge) {
                uiGoToScreen(SCREEN_GITHUB_TROPHIES);
            }
            
            if (downEdge) {
                uiGoToScreen(SCREEN_GITHUB_STATS);
            }
            break;

        case SCREEN_GITHUB_STATS:
            if (upEdge) {
                uiGoToScreen(SCREEN_GITHUB);
            }
            break;

        case SCREEN_GITHUB_TROPHIES:
            if (downEdge) {
                uiGoToScreen(SCREEN_GITHUB);
            }
            break;

        default:
            break;
    }
}

static void handleAppsMenuNavigation(bool upEdge, bool downEdge, bool leftEdge, bool rightEdge) {
    int selectedRow = selected / MENU_COLS;
    int selectedCol = selected % MENU_COLS;
    int totalRows = (menuTotalElements + MENU_COLS - 1) / MENU_COLS;

    if (upEdge) {
        if (selectedRow > 0) {
            selected -= MENU_COLS;
            selectedRow--;
            
            if (selectedRow < scroll) {
                scroll--;
            }
        }

        needsRedraw = true;
    }

    if (downEdge) {
        if (selectedRow < totalRows - 1) {
            selected += MENU_COLS;
            selectedRow++;

            if (selectedRow >= scroll + MENU_ROWS) {
                scroll++;
            }
        }

        needsRedraw = true;
    }

    if (leftEdge) {
        if (selectedCol > 0) {
            selected--;
            selectedCol--;
        }

        needsRedraw = true;
    }

    if (rightEdge) {
        if (selectedCol < MENU_COLS - 1 && selected < menuTotalElements - 1) {
            selected++;
            selectedCol++;
        }
        else {
            uiGoToScreen(SCREEN_BADGE);
            return;
        }

        needsRedraw = true;
    }
}

static void drawBadgeScreen() {
    fbDrawRect(4, 4, EINK_WIDTH - 8, EINK_HEIGHT - 8, false, FB_BLACK);
    fbDrawText(12, 16, "NAME", FB_BLACK, EINK_WIDTH - 24);
    fbDrawText(12, 32, "UHBADGE", FB_BLACK, 0);
}

static void drawContactScreen() {
    fbDrawText(8, 16, "CONTACT INFO", FB_BLACK, 0);
}

static void drawSettingsScreen() {
    fbDrawText(8, 16, "SETTINGS", FB_BLACK, 0);
    fbDrawText(8, 32, "WIFI / BLUETOOTH", FB_BLACK, EINK_WIDTH - 16);
}

static void drawGithubScreen() {
    fbDrawText(8, 16, "GITHUB BADGE", FB_BLACK, 0);
}

static void drawGithubStatsScreen() {
    fbDrawText(8, 16, "CONTRIBUTION GRAPH", FB_BLACK, EINK_WIDTH - 16);
}

static void drawGithubTrophiesScreen() {
    fbDrawText(8, 16, "TROPHIES", FB_BLACK, 0);
}

static void drawAppsMenuScreen() {
    int cellW = EINK_WIDTH / MENU_COLS - 2;
    int cellH = 48;

    for (int i = 0; i < MENU_VISIBLE && (scroll * MENU_COLS + i) < menuTotalElements; i++) {
        int index = scroll * MENU_COLS + i;
        int row = i / MENU_COLS;
        int col = i % MENU_COLS;

        int x = col * cellW;
        int y = row * cellH;

        bool isSelected = (index == selected);

        fbDrawRect(x + 2, y + 2, cellW - 4, cellH - 4, isSelected, isSelected ? FB_BLACK : FB_WHITE);

        if (!isSelected) {
            fbDrawRect(x + 2, y + 2, cellW - 4, cellH - 4, false, FB_BLACK);
        }

        fbDrawText(x + 8, y + cellH / 2 - 4, apps[index].name, isSelected ? FB_WHITE : FB_BLACK, cellW - 16);
    }
}

static void renderCurrentScreen() {
    fbClear();

    switch (currentScreen) {
        case SCREEN_BADGE:
            drawBadgeScreen();
            break;

        case SCREEN_CONTACT:
            drawContactScreen();
            break;

        case SCREEN_SETTINGS:
            drawSettingsScreen();
            break;

        case SCREEN_GITHUB:
            drawGithubScreen();
            break;

        case SCREEN_GITHUB_STATS:
            drawGithubStatsScreen();
            break;

        case SCREEN_GITHUB_TROPHIES:
            drawGithubTrophiesScreen();
            break;

        case SCREEN_APPS_MENU:
            drawAppsMenuScreen();
            break;
    }

    einkWriteFramebuffer(fbGetBuffer());
    einkUpdate(!firstFullRefreshDone ? false : true);

    firstFullRefreshDone = true;
}

void uiInit() {
    menuTotalElements = sizeof(apps) / sizeof(App);
    currentScreen = SCREEN_BADGE;
    needsRedraw = true;

    renderCurrentScreen();
}

void uiUpdate() {
    bool upEdge, downEdge, leftEdge, rightEdge;
    
    readButtonEdges(&upEdge, &downEdge, &leftEdge, &rightEdge);

    if (!upEdge && !downEdge && !leftEdge && !rightEdge) {
        return;
    }

    if (currentScreen == SCREEN_APPS_MENU) {
        handleAppsMenuNavigation(upEdge, downEdge, leftEdge, rightEdge);
    }
    else {
        handleCrossNavigation(upEdge, downEdge, leftEdge, rightEdge);
    }

    if (needsRedraw) {
        renderCurrentScreen();
        needsRedraw = false;
    }
}
