#include "ui.h"
#include "eink.h"
#include "../tools/storage.h"
#include "../tools/badgeData.h"
#include "../tools/contactsData.h"
#include "../tools/qrRender.h"
#include "../tools/nfc.h"

#include <string.h>
#include <stdio.h>

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

static int contactSelectedLink = 0;
static int contactVisibleLinkCount = 0;
static bool contactIsTransmitting = false;

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

static void truncateForLine(const char* text, char* out, size_t outSize, int maxChars) {
    size_t len = strlen(text);

    if ((int)len <= maxChars) {
        strncpy(out, text, outSize - 1);
        out[outSize - 1] = '\0';

        return;
    }

    int keep = maxChars - 3;

    if (keep < 1) {
        keep = 1;
    }

    if ((size_t)keep >= outSize) {
        keep = outSize - 1;
    }

    strncpy(out, text, keep);
    out[keep] = '\0';
    strncat(out, "...", outSize - strlen(out) - 1);
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
                contactSelectedLink = 0;
                contactIsTransmitting = false;
                
                uiGoToScreen(SCREEN_CONTACT);
            }

            if (leftEdge) {
                uiGoToScreen(SCREEN_APPS_MENU);
            }

            if (rightEdge) {
                uiGoToScreen(SCREEN_GITHUB);
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

static void handleContactsNavigation(bool upEdge, bool downEdge, bool leftEdge, bool rightEdge) {
    if (upEdge) {
        if (contactSelectedLink == 0) {
            nfcClear();
            contactIsTransmitting = false;

            uiGoToScreen(SCREEN_BADGE);
            
            return;
        }

        contactSelectedLink--;
        contactIsTransmitting = false;
        needsRedraw = true;
    }

    if (downEdge) {
        if (contactSelectedLink < contactVisibleLinkCount - 1) {
            contactSelectedLink++;
            contactIsTransmitting = false;
            needsRedraw = true;
        }
    }

    if (rightEdge) {
        ContactsData contacts;
        contactsDataLoad(&contacts);

        if (contactSelectedLink < contacts.linkCount) {
            nfcWriteUri(contacts.links[contactSelectedLink].value);
            contactIsTransmitting = true;

            needsRedraw = true;
        }
    }

    if (leftEdge) {
        nfcClear();
        contactIsTransmitting = false;

        needsRedraw = true;
    }
}

static void drawBadgeScreen() {
    BadgeData badge;
    badgeDataLoad(&badge);

    int centerX = EINK_WIDTH / 2;

    if (!badge.found) {
        fbDrawText(4, 8, "TOP LEFT", FB_BLACK, 0);
        fbDrawText(EINK_WIDTH - 4 - 8 * (FB_FONT_WIDTH + 1), 8, "TOP RIGHT", FB_BLACK, 0);
        fbDrawRect(centerX - BADGE_IMAGE_DIAMETER / 2, 24, BADGE_IMAGE_DIAMETER, BADGE_IMAGE_DIAMETER, false, FB_BLACK);
        fbDrawText(4, 90, "FIRST NAME", FB_BLACK, 0);
        fbDrawText(4, 102, "LAST NAME", FB_BLACK, 0);
        fbDrawText(4, 130, "MIDDLE TOP", FB_BLACK, 0);
        fbDrawText(4, 142, "MIDDLE BOTTOM", FB_BLACK, 0);
        fbDrawText(4, EINK_HEIGHT - 16, "BOTTOM", FB_BLACK, 0);

        return;
    }

    if (badge.topLeft[0] != '\0') {
        fbDrawText(4, 8, badge.topLeft, FB_BLACK, 0);
    }

    if (badge.topRight[0] != '\0') {
        int textWidth = strlen(badge.topRight) * (FB_FONT_WIDTH + 1);

        fbDrawText(EINK_WIDTH - 4 - textWidth, 8, badge.topRight, FB_BLACK, 0);
    }

    if (badge.imagePath[0] != '\0') {
        uint8_t imageBuf[BADGE_IMAGE_BUF_SIZE];
        size_t bytesRead = 0;

        if (storageReadBinaryFile(badge.imagePath, imageBuf, sizeof(imageBuf), &bytesRead) && bytesRead > 2) {
            uint8_t imgWidth = imageBuf[0];
            uint8_t imgHeight = imageBuf[1];

            if (imgWidth == BADGE_IMAGE_DIAMETER && imgHeight == BADGE_IMAGE_DIAMETER) {
                fbDrawBitmapCircular(centerX, 24 + BADGE_IMAGE_DIAMETER / 2, BADGE_IMAGE_DIAMETER, &imageBuf[2], FB_BLACK);
            }
        }
    }

    if (badge.firstName[0] != '\0') {
        fbDrawText(4, 24 + BADGE_IMAGE_DIAMETER + 8, badge.firstName, FB_BLACK, 0);
    }

    if (badge.lastName[0] != '\0') {
        fbDrawText(4, 24 + BADGE_IMAGE_DIAMETER + 20, badge.lastName, FB_BLACK, 0);
    }

    if (badge.middleTop[0] != '\0') {
        fbDrawText(4, EINK_HEIGHT - 60, badge.middleTop, FB_BLACK, EINK_WIDTH - 8);
    }

    if (badge.middleBottom[0] != '\0') {
        fbDrawText(4, EINK_HEIGHT - 44, badge.middleBottom, FB_BLACK, EINK_WIDTH - 8);
    }

    if (badge.bottom[0] != '\0') {
        fbDrawText(4, EINK_HEIGHT - 16, badge.bottom, FB_BLACK, EINK_WIDTH - 8);
    }
}

static void drawContactScreen() {
    ContactsData contacts;
    contactsDataLoad(&contacts);

    if (!contacts.found || contacts.linkCount == 0) {
        fbDrawText(8, 16, "CONTACT INFO", FB_BLACK, 0);
        fbDrawText(8, 32, "NO LINKS FOUND", FB_BLACK, EINK_WIDTH - 16);

        contactVisibleLinkCount = 0;

        return;
    }

    int linksStartY = 8;

    if (contacts.hasQrcode) {
        int qrHeight = qrRenderDraw(contacts.qrcodeData, 4);

        if (qrHeight > 0) {
            linksStartY = 4 + qrHeight + 8;
        }
        else {
            fbDrawText(8, 4, "QR: LINK TOO LONG", FB_BLACK, EINK_WIDTH - 16);
            linksStartY = 20;
        }
    }

    int availableHeight = EINK_HEIGHT - linksStartY - 4;
    int maxVisibleLinks = availableHeight / CONTACTS_LINE_HEIGHT;

    if (maxVisibleLinks > CONTACTS_MAX_LINKS) {
        maxVisibleLinks = CONTACTS_MAX_LINKS;
    }

    int visibleCount = contacts.linkCount;

    if (visibleCount > maxVisibleLinks) {
        visibleCount = maxVisibleLinks;
    }

    contactVisibleLinkCount = visibleCount;

    if (contactSelectedLink >= visibleCount) {
        contactSelectedLink = visibleCount > 0 ? visibleCount - 1 : 0;
    }

    for (int i = 0; i < visibleCount; i++) {
        int y = linksStartY + i * CONTACTS_LINE_HEIGHT;
        bool isSelected = (i == contactSelectedLink);

        char combined[CONTACTS_LABEL_MAX_LEN + CONTACTS_VALUE_MAX_LEN + 4];
        snprintf(combined, sizeof(combined), "%s: %s", contacts.links[i].label, contacts.links[i].value);

        char line[CONTACTS_MAX_LINE_CHARS + 4];
        truncateForLine(combined, line, sizeof(line), CONTACTS_MAX_LINE_CHARS);

        if (isSelected) {
            fbDrawRect(0, y - 1, EINK_WIDTH, CONTACTS_LINE_HEIGHT, true, FB_BLACK);
            fbDrawText(4, y, line, FB_WHITE, 0);

            if (contactIsTransmitting) {
                fbDrawText(EINK_WIDTH - 4 - 2 * (FB_FONT_WIDTH + 1), y, "TX", FB_WHITE, 0);
            }
        }
        else {
            fbDrawText(4, y, line, FB_BLACK, 0);
        }
    }
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
    else if (currentScreen == SCREEN_CONTACT) {
        handleContactsNavigation(upEdge, downEdge, leftEdge, rightEdge);
    }
    else {
        handleCrossNavigation(upEdge, downEdge, leftEdge, rightEdge);
    }

    if (needsRedraw) {
        renderCurrentScreen();
        needsRedraw = false;
    }
}
