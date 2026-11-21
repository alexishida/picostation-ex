#include "filesystem.h"
#include "cdrom.h"
#include <string.h>

enum { ISO_SECTOR_BYTES = 2048 };
static uint32_t rootSector[ISO_SECTOR_BYTES / sizeof(uint32_t)];
static uint32_t rootLba;
static uint32_t rootSize;

uint32_t getRootDirLba(uint8_t *pvdSector, uint32_t *lba)
{
    *lba = int32_LM(pvdSector, 158);
    return int32_LM(pvdSector, 166);
}

int parseDirRecord(uint8_t *record, uint8_t *recordLength, DirectoryEntry *entry)
{
    *recordLength = record[0];
    if (*recordLength < 34)
        return 1;
    const unsigned nameLength = record[32];
    if (!nameLength || 33 + nameLength > *recordLength)
        return 1;
    entry->lba = int32_LM(record, 2);
    entry->length = int32_LM(record, 10);
    if (nameLength == 1 && record[33] <= 1)
        strcpy(entry->name, record[33] ? ".." : ".");
    else
    {
        memcpy(entry->name, record + 33, nameLength);
        entry->name[nameLength] = 0;
    }
    return 0;
}

int getRootDirData(void *output)
{
    uint32_t descriptor[ISO_SECTOR_BYTES / sizeof(uint32_t)];
    uint8_t *pvd = (uint8_t *)descriptor;
    rootSize = 0;
    startCDROMRead(16, descriptor, 1, ISO_SECTOR_BYTES, true, true);
    if (!waitingForInt5 || pvd[0] != 1 || memcmp(pvd + 1, "CD001", 5) ||
        memcmp(pvd + 8, "PLAYSTATION", 11))
        return -1;

    const uint32_t size = getRootDirLba(pvd, &rootLba);
    const uint32_t volumeSectors = int32_LM(pvd, 80);
    if (!size || rootLba >= volumeSectors || (size - 1) / ISO_SECTOR_BYTES >= volumeSectors - rootLba)
        return -1;
    startCDROMRead(rootLba, rootSector, 1, ISO_SECTOR_BYTES, true, true);
    if (!waitingForInt5)
        return -1;
    rootSize = size;
    if (output != rootSector)
        memcpy(output, rootSector, ISO_SECTOR_BYTES);
    return 0;
}

int initFilesystem(void)
{
    return getRootDirData(rootSector);
}

static bool findRootEntry(const char *filename, DirectoryEntry *entry)
{
    uint32_t nextSector[ISO_SECTOR_BYTES / sizeof(uint32_t)];
    uint8_t *data = (uint8_t *)rootSector;
    uint32_t lba = rootLba;
    uint32_t remaining = rootSize;

    while (remaining)
    {
        const unsigned bytes = remaining < ISO_SECTOR_BYTES ? remaining : ISO_SECTOR_BYTES;
        for (unsigned offset = 0; offset < bytes && data[offset]; )
        {
            uint8_t length;
            if (data[offset] > bytes - offset || parseDirRecord(data + offset, &length, entry))
                return false;
            if (!strcmp(entry->name, filename))
                return true;
            offset += length;
        }
        remaining -= bytes;
        if (remaining)
        {
            startCDROMRead(++lba, nextSector, 1, ISO_SECTOR_BYTES, true, true);
            if (!waitingForInt5)
                return false;
            data = (uint8_t *)nextSector;
        }
    }
    return false;
}

uint32_t getLbaToFile(const char *filename)
{
    DirectoryEntry entry;
    return findRootEntry(filename, &entry) ? entry.lba : 0;
}

bool getFileInfo(const char *filename, DirectoryEntry *entry)
{
    return initFilesystem() == 0 && findRootEntry(filename, entry);
}
