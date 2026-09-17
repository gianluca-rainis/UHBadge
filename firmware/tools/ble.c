#include "ble.h"
#include "btstack_config.h"
#include "pico-sdk/src/rp2_common/pico_cyw43_arch/include/pico/cyw43_arch.h"

#include <string.h>

#define UHBADGE_BLE_COMPANY_ID 0xFFFF
#define UHBADGE_BLE_MAGIC "UHBG" // recognize other UHBadges

static btstack_packet_callback_registration_t hciEventCallbackRegistration;
static NearbyBadge nearbyBadges[BLE_MAX_NEARBY_BADGES];
static int nearbyBadgeCount = 0;

static uint8_t advData[31];
static uint8_t advDataLen = 0;

static void buildAdvertisingData(const char* badgeName) {
    size_t nameLen = strlen(badgeName);

    if (nameLen > BLE_MAX_NAME_LEN - 1) {
        nameLen = BLE_MAX_NAME_LEN - 1;
    }

    uint8_t idx = 0;

    advData[idx++] = 0x02;
    advData[idx++] = 0x01;
    advData[idx++] = 0x06;

    uint8_t payloadLen = 2 + 4 + nameLen; // Company ID + magic + badge name

    advData[idx++] = payloadLen + 1;
    advData[idx++] = 0xFF;
    advData[idx++] = UHBADGE_BLE_COMPANY_ID & 0xFF;
    advData[idx++] = (UHBADGE_BLE_COMPANY_ID >> 8) & 0xFF;

    memcpy(&advData[idx], UHBADGE_BLE_MAGIC, 4);
    idx += 4;
    memcpy(&advData[idx], badgeName, nameLen);
    idx += nameLen;

    advDataLen = idx;
}

static bool parseUhbadgeAdvertising(const uint8_t* data, uint8_t len, char* nameOut) {
    uint8_t i = 0;

    while (i < len) {
        uint8_t structLen = data[i];

        if (structLen == 0) {
            break;
        }

        uint8_t adType = data[i + 1];
        const uint8_t* adData = &data[i + 2];
        uint8_t adDataLen = structLen - 1;

        if (adType == 0xFF && adDataLen >= 6 && memcmp(&adData[2], UHBADGE_BLE_MAGIC, 4) == 0) {
            uint8_t nameLen = adDataLen - 6;

            if (nameLen > BLE_MAX_NAME_LEN - 1) {
                nameLen = BLE_MAX_NAME_LEN - 1;
            }

            memcpy(nameOut, &adData[6], nameLen);
            nameOut[nameLen] = '\0';

            return true;
        }

        i += structLen + 1;
    }

    return false;
}

static void upsertNearbyBadge(const char* name, int8_t rssi) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    for (int i = 0; i < nearbyBadgeCount; i++) {
        if (strncmp(nearbyBadges[i].name, name, BLE_MAX_NAME_LEN) == 0) {
            nearbyBadges[i].rssi = rssi;
            nearbyBadges[i].lastSeenMs = now;

            return;
        }
    }

    if (nearbyBadgeCount < BLE_MAX_NEARBY_BADGES) {
        strncpy(nearbyBadges[nearbyBadgeCount].name, name, BLE_MAX_NAME_LEN - 1);
        nearbyBadges[nearbyBadgeCount].rssi = rssi;
        nearbyBadges[nearbyBadgeCount].lastSeenMs = now;
        nearbyBadgeCount++;
    }
}

static void removeStaleBadges() {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    int writeIdx = 0;

    for (int readIdx = 0; readIdx < nearbyBadgeCount; readIdx++) {
        if (now - nearbyBadges[readIdx].lastSeenMs < BLE_NEARBY_TIMEOUT_MS) {
            if (writeIdx != readIdx) {
                nearbyBadges[writeIdx] = nearbyBadges[readIdx];
            }

            writeIdx++;
        }
    }

    nearbyBadgeCount = writeIdx;
}

static void packetHandler(uint8_t packetType, uint16_t channel, uint8_t* packet, uint16_t size) {
    if (packetType != HCI_EVENT_PACKET) {
        return;
    }

    uint8_t eventType = hci_event_packet_get_type(packet);

    if (eventType == BTSTACK_EVENT_STATE) {
        return;
    }

    if (eventType == GAP_EVENT_ADVERTISING_REPORT) {
        int8_t rssi = gap_event_advertising_report_get_rssi(packet);
        uint8_t dataLen = gap_event_advertising_report_get_data_length(packet);
        const uint8_t* data = gap_event_advertising_report_get_data(packet);

        char name[BLE_MAX_NAME_LEN];

        if (parseUhbadgeAdvertising(data, dataLen, name)) {
            upsertNearbyBadge(name, rssi);
        }
    }
}

bool bleInit() {
    if (cyw43_arch_init()) {
        return false;
    }

    l2cap_init();
    sm_init();

    hciEventCallbackRegistration.callback = &packetHandler;
    hci_add_event_handler(&hciEventCallbackRegistration);

    hci_power_control(HCI_POWER_ON);

    return true;
}

bool bleStartAdvertising(const char* badgeName) {
    buildAdvertisingData(badgeName);

    uint16_t advIntervalMin = 0x0800;
    uint16_t advIntervalMax = 0x0800;
    bd_addr_t nullAddr;

    memset(nullAddr, 0, sizeof(nullAddr));

    gap_advertisements_set_params(advIntervalMin, advIntervalMax, 0x03, 0, nullAddr, 0x07, 0x00);
    gap_advertisements_set_data(advDataLen, advData);
    gap_advertisements_enable(1);

    return true;
}

bool bleStartScanning() {
    gap_set_scan_parameters(0, 0x0030, 0x0030);
    gap_start_scan();

    return true;
}

void blePoll() {
    btstack_run_loop_poll_data_sources_from_irq();

    removeStaleBadges();
}

int bleGetNearbyBadges(NearbyBadge* out) {
    memcpy(out, nearbyBadges, sizeof(NearbyBadge) * nearbyBadgeCount);

    return nearbyBadgeCount;
}
