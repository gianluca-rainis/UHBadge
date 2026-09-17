#include "wifi.h"
#include "storage.h"
#include "pico-sdk/src/rp2_common/pico_cyw43_arch/include/pico/cyw43_arch.h"

#include <string.h>
#include <stdio.h>

static WifiState currentState = WIFI_STATE_DISCONNECTED;

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
