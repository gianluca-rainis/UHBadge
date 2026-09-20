#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef UHBADGE_BLE
    #define UHBADGE_BLE

    #define BLE_MAX_NAME_LEN 20 // Limit of the badge name length
    #define BLE_MAX_NEARBY_BADGES 16 // Max number of nearby badges to track (for memory reasons)
    #define BLE_NEARBY_TIMEOUT_MS 10000 // Time to consider a badge "nearby" since last seen (in milliseconds)

    typedef struct {
        char name[BLE_MAX_NAME_LEN];
        int8_t rssi; // power of the signal (->0 = strong)
        uint32_t lastSeenMs; // timestamp from last advertising
    } NearbyBadge;

    bool bleInit();
    bool bleStartAdvertising(const char* badgeName);
    bool bleStartScanning();
    void blePoll();
    int bleGetNearbyBadges(NearbyBadge* out);
    bool bleIsEnabled();
    void bleSetEnabled(bool enabled);
#endif
