#include "wifi.h"
#include "storage.h"
#include "pico/cyw43_arch.h"

#include <string.h>
#include <stdio.h>

static WifiState currentState = WIFI_STATE_DISCONNECTED;
static bool wifiEnabledFlag = false;

typedef struct {
    char (*ssidsOut)[WIFI_SSID_MAX_LEN];
    int maxCount;
    int foundCount;
} ScanContext;

bool wifiInit() {
    if (cyw43_arch_init()) {
        currentState = WIFI_STATE_FAILED;

        return false;
    }

    cyw43_arch_enable_sta_mode();

    currentState = WIFI_STATE_DISCONNECTED;

    return true;
}

bool wifiConnect(const char* ssid, const char* password) {
    currentState = WIFI_STATE_CONNECTING;

    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS);

    currentState = (result == 0) ? WIFI_STATE_CONNECTED : WIFI_STATE_FAILED;

    return result == 0;
}

bool wifiConnectFromStorage() {
    char content[WIFI_SSID_MAX_LEN + WIFI_PASSWORD_MAX_LEN + 4];

    if (!storageReadTextFile("/wifi.txt", content, sizeof(content))) {
        currentState = WIFI_STATE_FAILED;

        return false;
    }

    char ssid[WIFI_SSID_MAX_LEN] = {0};
    char password[WIFI_PASSWORD_MAX_LEN] = {0};

    char* line1 = strtok(content, "\r\n");
    char* line2 = strtok(NULL, "\r\n");

    if (line1 == NULL || line2 == NULL) {
        currentState = WIFI_STATE_FAILED;

        return false; // /wifi.txt without SSID or password
    }

    strncpy(ssid, line1, WIFI_SSID_MAX_LEN - 1);
    strncpy(password, line2, WIFI_PASSWORD_MAX_LEN - 1);

    return wifiConnect(ssid, password);
}

WifiState wifiGetState() {
    return currentState;
}

void wifiPoll() {
    cyw43_arch_poll();
}

bool wifiIsEnabled() {
    return wifiEnabledFlag;
}

void wifiSetEnabled(bool enabled) {
    wifiEnabledFlag = enabled;

    if (!enabled) {
        currentState = WIFI_STATE_DISCONNECTED;
    }
}

static int scanResultCallback(void* env, const cyw43_ev_scan_result_t* result) {
    ScanContext* ctx = (ScanContext*)env;

    if (result == NULL || result->ssid_len == 0) {
        return 0;
    }

    char ssid[WIFI_SSID_MAX_LEN];
    size_t len = result->ssid_len;

    if (len >= sizeof(ssid)) {
        len = sizeof(ssid) - 1;
    }

    memcpy(ssid, result->ssid, len);
    ssid[len] = '\0';

    for (int i = 0; i < ctx->foundCount; i++) {
        if (strcmp(ctx->ssidsOut[i], ssid) == 0) {
            return 0;
        }
    }

    if (ctx->foundCount < ctx->maxCount) {
        strncpy(ctx->ssidsOut[ctx->foundCount], ssid, WIFI_SSID_MAX_LEN - 1);
        ctx->ssidsOut[ctx->foundCount][WIFI_SSID_MAX_LEN - 1] = '\0';
        ctx->foundCount++;
    }

    return 0;
}

int wifiScanNetworks(char ssidsOut[][WIFI_SSID_MAX_LEN], int maxCount) {
    ScanContext ctx = { .ssidsOut = ssidsOut, .maxCount = maxCount, .foundCount = 0 };

    cyw43_wifi_scan_options_t scanOptions;
    memset(&scanOptions, 0, sizeof(scanOptions));

    int err = cyw43_wifi_scan(&cyw43_state, &scanOptions, &ctx, scanResultCallback);

    if (err != 0) {
        return 0;
    }

    uint32_t start = to_ms_since_boot(get_absolute_time());

    while (cyw43_wifi_scan_active(&cyw43_state)) {
        cyw43_arch_poll();

        if (to_ms_since_boot(get_absolute_time()) - start > WIFI_SCAN_TIMEOUT_MS) {
            break;
        }

        sleep_ms(50);
    }

    return ctx.foundCount;
}
