#include "wifiNetworks.h"
#include "storage.h"

#include <string.h>
#include <stdio.h>

#define WIFI_NETWORKS_JSON_MAX_LEN 2048

static bool findJsonStringFieldBounded(const char* searchFrom, const char* searchEnd, const char* key, char* out, size_t outSize) {
    char needle[32];
    snprintf(needle, sizeof(needle), "\"%s\"", key);

    const char* keyPos = strstr(searchFrom, needle);

    if (keyPos == NULL || keyPos >= searchEnd) {
        return false;
    }

    const char* colon = strchr(keyPos, ':');

    if (colon == NULL || colon >= searchEnd) {
        return false;
    }

    const char* valueStart = strchr(colon, '"');

    if (valueStart == NULL || valueStart >= searchEnd) {
        return false;
    }

    valueStart++;

    const char* valueEnd = strchr(valueStart, '"');

    if (valueEnd == NULL || valueEnd > searchEnd) {
        return false;
    }

    size_t len = valueEnd - valueStart;

    if (len >= outSize) {
        len = outSize - 1;
    }

    memcpy(out, valueStart, len);
    out[len] = '\0';

    return true;
}

void wifiNetworksLoad(SavedNetworksList* out) {
    memset(out, 0, sizeof(SavedNetworksList));

    static char jsonBuf[WIFI_NETWORKS_JSON_MAX_LEN];

    if (!storageReadTextFile(WIFI_NETWORKS_JSON_PATH, jsonBuf, sizeof(jsonBuf))) {
        return;
    }

    const char* arrayStart = strchr(jsonBuf, '[');
    const char* arrayEnd = arrayStart != NULL ? strrchr(jsonBuf, ']') : NULL;

    if (arrayStart == NULL || arrayEnd == NULL) {
        return;
    }

    const char* p = arrayStart;

    while (out->count < WIFI_NETWORKS_MAX) {
        const char* objStart = strchr(p, '{');

        if (objStart == NULL || objStart >= arrayEnd) {
            break;
        }

        const char* objEnd = strchr(objStart, '}');

        if (objEnd == NULL || objEnd > arrayEnd) {
            break;
        }

        SavedNetwork* net = &out->networks[out->count];
        bool hasSsid = findJsonStringFieldBounded(objStart, objEnd, "ssid", net->ssid, sizeof(net->ssid));
        bool hasPassword = findJsonStringFieldBounded(objStart, objEnd, "password", net->password, sizeof(net->password));

        if (hasSsid && hasPassword) {
            out->count++;
        }

        p = objEnd + 1;
    }
}

bool wifiNetworksFind(const SavedNetworksList* list, const char* ssid, char* passwordOut) {
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->networks[i].ssid, ssid) == 0) {
            strncpy(passwordOut, list->networks[i].password, WIFI_NETWORKS_PASSWORD_MAX_LEN - 1);
            passwordOut[WIFI_NETWORKS_PASSWORD_MAX_LEN - 1] = '\0';
            
            return true;
        }
    }

    return false;
}
