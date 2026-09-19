#include "contactsData.h"
#include "storage.h"

#include <string.h>
#include <stdio.h>

#define CONTACTS_JSON_MAX_LEN 4096

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

static void parseLinksArray(const char* arrayStart, const char* arrayEnd, ContactsData* out) {
    const char* p = arrayStart;

    while (out->linkCount < CONTACTS_MAX_LINKS) {
        const char* objStart = strchr(p, '{');

        if (objStart == NULL || objStart >= arrayEnd) {
            break;
        }

        const char* objEnd = strchr(objStart, '}');

        if (objEnd == NULL || objEnd > arrayEnd) {
            break;
        }

        ContactLink* link = &out->links[out->linkCount];
        bool hasLabel = findJsonStringFieldBounded(objStart, objEnd, "label", link->label, sizeof(link->label));
        bool hasValue = findJsonStringFieldBounded(objStart, objEnd, "value", link->value, sizeof(link->value));

        if (hasLabel && hasValue) {
            out->linkCount++;
        }

        p = objEnd + 1;
    }
}

void contactsDataLoad(ContactsData* out) {
    memset(out, 0, sizeof(ContactsData));
    out->found = false;

    static char jsonBuf[CONTACTS_JSON_MAX_LEN];
    
    if (!storageReadTextFile(CONTACTS_JSON_PATH, jsonBuf, sizeof(jsonBuf))) {
        return;
    }

    out->found = true;

    const char* jsonEnd = jsonBuf + strlen(jsonBuf);
    out->hasQrcode = findJsonStringFieldBounded(jsonBuf, jsonEnd, "qrcode", out->qrcodeData, sizeof(out->qrcodeData));

    const char* linksKey = strstr(jsonBuf, "\"links\"");

    if (linksKey != NULL) {
        const char* arrayStart = strchr(linksKey, '[');
        const char* arrayEnd = arrayStart != NULL ? strchr(arrayStart, ']') : NULL;

        if (arrayStart != NULL && arrayEnd != NULL) {
            parseLinksArray(arrayStart, arrayEnd, out);
        }
    }
}
