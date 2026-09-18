#include "badgeData.h"
#include "storage.h"

#include <string.h>
#include <stdio.h>

#define BADGE_JSON_MAX_LEN 2048

static bool findJsonStringField(const char* json, const char* key, char* out, size_t outSize) {
    char needle[64];

    snprintf(needle, sizeof(needle), "\"%s\"", key);

    const char* keyPos = strstr(json, needle);

    if (keyPos == NULL) {
        return false;
    }

    const char* colon = strchr(keyPos, ':');

    if (colon == NULL) {
        return false;
    }

    const char* valueStart = strchr(colon, "\"");

    if (valueStart == NULL) {
        return false;
    }

    valueStart++;

    const char* valueEnd = strchr(valueStart, "\"");

    if (valueEnd == NULL) {
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

static void parseBadgeJson(const char* json, BadgeData* out) {
    findJsonStringField(json, "topLeft", out->topLeft, sizeof(out->topLeft));
    findJsonStringField(json, "topRight", out->topRight, sizeof(out->topRight));
    findJsonStringField(json, "image", out->imagePath, sizeof(out->imagePath));
    findJsonStringField(json, "firstName", out->firstName, sizeof(out->firstName));
    findJsonStringField(json, "lastName", out->lastName, sizeof(out->lastName));
    findJsonStringField(json, "middleTop", out->middleTop, sizeof(out->middleTop));
    findJsonStringField(json, "middleBottom", out->middleBottom, sizeof(out->middleBottom));
    findJsonStringField(json, "bottom", out->bottom, sizeof(out->bottom));
}

void badgeDataLoad(BadgeData* out) {
    memset(out, 0, sizeof(BadgeData));
    out->found = false;

    StorageEntry entries[STORAGE_MAX_ENTRIES];
    int count = storageListDir(BADGE_DATA_FOLDER, entries);

    for (int i = 0; i < count; i++) {
        if (entries[i].isDirectory) {
            continue;
        }

        size_t nameLen = strlen(entries[i].name);

        if (nameLen < 5) {
            continue;
        }

        if (strcmp(entries[i].name + nameLen - 5, ".json") != 0) {
            continue;
        }

        char fullPath[BADGE_IMAGE_PATH_MAX_LEN];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", BADGE_DATA_FOLDER, entries[i].name);

        static char jsonBuf[BADGE_JSON_MAX_LEN];
        
        if (!storageReadTextFile(fullPath, jsonBuf, sizeof(jsonBuf))) {
            continue;
        }

        parseBadgeJson(jsonBuf, out);
        out->found = true;
        
        return;
    }
}
