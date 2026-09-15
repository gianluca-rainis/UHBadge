#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef UHBADGE_STORAGE
    #define UHBADGE_STORAGE

    #define STORAGE_MAX_ENTRIES 32
    #define STORAGE_MAX_NAME_LEN 64

    typedef struct {
        char name[STORAGE_MAX_NAME_LEN];
        bool isDirectory;
    } StorageEntry;
    
    bool storageInit();
    bool storageReadTextFile(const char* path, char* buf, size_t bufSize);
    bool storageWriteTextFile(const char* path, const char* text);
    int storageListDir(const char* path, StorageEntry* out);
    bool storageExists(const char* path);
#endif
