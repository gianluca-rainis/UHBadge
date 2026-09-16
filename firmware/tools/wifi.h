#include <stdbool.h>
#include <stdint.h>

#ifndef UHBADGE_WIFI
    #define UHBADGE_WIFI

    #define WIFI_SSID_MAX_LEN 64
    #define WIFI_PASSWORD_MAX_LEN 64
    #define WIFI_CONNECT_TIMEOUT_MS 15000

    typedef enum {
        WIFI_STATE_DISCONNECTED,
        WIFI_STATE_CONNECTING,
        WIFI_STATE_CONNECTED,
        WIFI_STATE_FAILED
    } WifiState;

    bool wifiInit();
    bool wifiConnect(const char* ssid, const char* password);
    bool wifiConnectFromStorage();
    WifiState wifiGetState();
    void wifiPoll();
#endif
