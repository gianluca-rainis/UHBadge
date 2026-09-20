#include <stdbool.h>

#ifndef UHBADGE_WIFI_NETWORKS
    #define UHBADGE_WIFI_NETWORKS

    #define WIFI_NETWORKS_JSON_PATH "/UHBadge-wifi/networks.json"
    #define WIFI_NETWORKS_MAX 20
    #define WIFI_NETWORKS_SSID_MAX_LEN 33
    #define WIFI_NETWORKS_PASSWORD_MAX_LEN 64

    typedef struct {
        char ssid[WIFI_NETWORKS_SSID_MAX_LEN];
        char password[WIFI_NETWORKS_PASSWORD_MAX_LEN];
    } SavedNetwork;

    typedef struct {
        SavedNetwork networks[WIFI_NETWORKS_MAX];
        int count;
    } SavedNetworksList;

    void wifiNetworksLoad(SavedNetworksList* out);
    bool wifiNetworksFind(const SavedNetworksList* list, const char* ssid, char* passwordOut);
#endif
