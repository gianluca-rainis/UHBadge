#include "storage.h"
#include "../lib/fatfs/ff.h"
#include "sd.h"

#include <string.h>

static FATFS fatFs;
static bool mounted = false;

bool storageInit() {
    FRESULT res = f_mount(&fatFs, "", 1);
    mounted = (res == FR_OK);

    return mounted;
}

bool storageReadTextFile(const char* path, char* buf, size_t bufSize) {
    if (!mounted || bufSize == 0) {
        return false;
    }

    FIL file;

    if (f_open(&file, path, FA_READ) != FR_OK) {
        return false;
    }

    UINT bytesRead = 0;
    FRESULT res = f_read(&file, buf, bufSize - 1, &bytesRead);

    f_close(&file);

    if (res != FR_OK) {
        return false;
    }

    buf[bytesRead] = '\0';

    return true;
}

bool storageWriteTextFile(const char* path, const char* text) {
    if (!mounted) {
        return false;
    }

    FIL file;

    if (f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        return false;
    }

    UINT bytesWritten = 0;
    FRESULT res = f_write(&file, text, strlen(text), &bytesWritten);

    f_close(&file);

    return (res == FR_OK) && (bytesWritten == strlen(text));
}

int storageListDir(const char* path, StorageEntry* out) {
    if (!mounted) {
        return 0;
    }

    DIR dir;

    if (f_opendir(&dir, path) != FR_OK) {
        return 0;
    }

    int count = 0;
    FILINFO info;

    while (count < STORAGE_MAX_ENTRIES) {
        if (f_readdir(&dir, &info) != FR_OK || info.fname[0] == '\0') {
            break;
        }

        strncpy(out[count].name, info.fname, STORAGE_MAX_NAME_LEN - 1);
        out[count].name[STORAGE_MAX_NAME_LEN - 1] = '\0';
        out[count].isDirectory = (info.fattrib & AM_DIR) != 0;

        count++;
    }

    f_closedir(&dir);

    return count;
}

bool storageReadBinaryFile(const char* path, uint8_t* buf, size_t bufSize, size_t* bytesRead) {
    if (!mounted) {
        return false;
    }

    FIL file;

    if (f_open(&file, path, FA_READ) != FR_OK) {
        return false;
    }

    UINT read = 0;
    FRESULT res = f_read(&file, buf, bufSize, &read);
    f_close(&file);

    if (res != FR_OK) {
        return false;
    }

    *bytesRead = read;

    return true;
}

bool storageExists(const char* path) {
    if (!mounted) {
        return false;
    }

    FILINFO info;
    
    return f_stat(path, &info) == FR_OK;
}

DWORD get_fattime(void) {
    return ((DWORD)(2026 - 1980) << 25) | ((DWORD)1 << 21) | ((DWORD)1 << 16);
}
