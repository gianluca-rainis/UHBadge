#include <stdbool.h>
#include <stddef.h>

#ifndef UHBADGE_BADGE_DATA
    #define UHBADGE_BADGE_DATA

    #define BADGE_FIELD_MAX_LEN 40
    #define BADGE_IMAGE_PATH_MAX_LEN 128
    #define BADGE_DATA_PATH "/UHBadge-main/badge.json"

    typedef struct {
        char topLeft[BADGE_FIELD_MAX_LEN];
        char topRight[BADGE_FIELD_MAX_LEN];
        char imagePath[BADGE_IMAGE_PATH_MAX_LEN];
        char firstName[BADGE_FIELD_MAX_LEN];
        char lastName[BADGE_FIELD_MAX_LEN];
        char middleTop[BADGE_FIELD_MAX_LEN];
        char middleBottom[BADGE_FIELD_MAX_LEN];
        char bottom[BADGE_FIELD_MAX_LEN];
        bool found;
    } BadgeData;

    void badgeDataLoad(BadgeData* out);
#endif
