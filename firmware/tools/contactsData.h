#include <stdbool.h>

#ifndef UHBADGE_CONTACTS_DATA
    #define UHBADGE_CONTACTS_DATA

    #include "../ui/framebuffer.h"

    #define CONTACTS_JSON_PATH "/UHBadge-main/contacts.json"
    #define CONTACTS_MAX_LINKS 20
    #define CONTACTS_LABEL_MAX_LEN 24
    #define CONTACTS_VALUE_MAX_LEN 80
    #define CONTACTS_QR_MAX_LEN 128

    #define CONTACTS_MAX_LINE_CHARS ((EINK_WIDTH-8) / (FB_FONT_WIDTH+1))
    #define CONTACTS_LINE_HEIGHT 12

    typedef struct {
        char label[CONTACTS_LABEL_MAX_LEN];
        char value[CONTACTS_VALUE_MAX_LEN];
    } ContactLink;

    typedef struct {
        char qrcodeData[CONTACTS_QR_MAX_LEN];
        bool hasQrcode;
        ContactLink links[CONTACTS_MAX_LINKS];
        int linkCount;
        bool found;
    } ContactsData;

    void contactsDataLoad(ContactsData* out);
#endif
