// diskio.c for FatFs
// FatFs (ff.h/ff.c/diskio.h) from http://elm-chan.org/fsw/ff/00index_e.html
// in firmware/lib/fatfs/

#include "../lib/fatfs/ff.h"
#include "../lib/fatfs/diskio.h"
#include "sd.h"

static bool sdInitialized = false;

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != 0) {
        return STA_NOINIT;
    }

    return sdInitialized ? 0 : STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != 0) {
        return STA_NOINIT;
    }

    sdInitialized = sdSpiInit();

    return sdInitialized ? 0 : STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !sdInitialized) {
        return RES_NOTRDY;
    }

    for (UINT i = 0; i < count; i++) {
        if (!sdSpiReadBlock(sector + i, buff + i * SD_BLOCK_SIZE)) {
            return RES_ERROR;
        }
    }
    
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !sdInitialized) {
        return RES_NOTRDY;
    }

    for (UINT i = 0; i < count; i++) {
        if (!sdSpiWriteBlock(sector + i, buff + i * SD_BLOCK_SIZE)) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    if (pdrv != 0) {
        return RES_PARERR;
    }

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = SD_BLOCK_SIZE;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}
