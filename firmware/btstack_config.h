/* 
 * Minimal configuration file for BTstack
 * Only BLE (no Bluetooth Classic)
*/

#ifndef BTSTACK_CONFIG_H
    #define BTSTACK_CONFIG_H

    #define HAVE_EMBEDDED_TIME_MS
    #define HCI_ACL_PAYLOAD_SIZE 256

    #define ENABLE_PRINTF_HEXDUMP

    #define HCI_OUTGOING_PRE_BUFFER_SIZE 4
    #define HCI_ACL_CHUNK_SIZE_ALIGNMENT 4

    #define ENABLE_LE_PERIPHERAL // advertising
    #define ENABLE_LE_CENTRAL // scanning

    #define MAX_ATT_DB_SIZE 64 // 64 bytes for GATT database

    #define MAX_NR_HCI_CONNECTIONS 1
    #define MAX_NR_SM_LOOKUP_ENTRIES 3
    #define MAX_NR_WHITELIST_ENTRIES 1
    #define MAX_NR_LE_DEVICE_DB_ENTRIES 1

    #define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 0
    #define MAX_NR_SERVICE_RECORD_ITEMS 0
    #define MAX_NR_HFP_CONNECTIONS 0

    #define NVM_NUM_DEVICE_DB_ENTRIES 1
    #define NVM_NUM_LINK_KEYS 0
#endif
