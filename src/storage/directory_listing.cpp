#include "directory_listing.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "global.h"
#include "ff.h"
#include "listingBuilder.h"
#include "menu_protocol.h"
#include "logging.h"

#if DEBUG_FILEIO
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while (0)
#endif

namespace picostation
{

namespace
{
    char currentDirectory[c_maxFilePathLength + 1];
    listingBuilder* fileListing;
}  // namespace

static FIL cover_fp;
static bool coverRgb15;
static FIL cfg_fp;
static FATFS s_fatFS;
static bool sd_present = false;

static bool __time_critical_func(isVisibleEntry)(const FILINFO& entry)
{
    if (entry.fattrib & AM_HID)
    {
        return false;
    }
    size_t length = strlen(entry.fname);
    return (entry.fattrib & AM_DIR) ||
           (length > 4 && strcasecmp(entry.fname + length - 4, ".cue") == 0);
}

void __time_critical_func(DirectoryListing::init)()
{
    sd_present = false;
    
    memset(&cover_fp, 0, sizeof(FIL));
    memset(&cfg_fp, 0, sizeof(FIL));
    
    FRESULT fr = f_mount(&s_fatFS, "", 1);
	
	if (FR_OK == fr)
	{
		sd_present = true;
		//panic("f_mount error: (%d)\n", fr);
	}
    
    fileListing = new listingBuilder();
    gotoRoot();
}

void __time_critical_func(DirectoryListing::gotoRoot)()
{ 
    currentDirectory[0] = '\0';
}

bool __time_critical_func(DirectoryListing::gotoDirectory)(const uint32_t index)
{ 
    char newFolder[c_maxFilePathLength + 1];
    bool isDirectory = false;
    bool result = getDirectoryEntry(index, newFolder, &isDirectory) && isDirectory &&
                  combinePaths(currentDirectory, newFolder, currentDirectory);
    
    DEBUG_PRINT("gotoDirectory: %s\n", currentDirectory);
    return result;
}

bool __time_critical_func(DirectoryListing::getPath)(uint32_t index, char* filePath, bool* isDirectory)
{
    char name[c_maxFilePathLength + 1];
    return getDirectoryEntry(index, name, isDirectory) &&
           combinePaths(currentDirectory, name, filePath);
}

void __time_critical_func(DirectoryListing::gotoParentDirectory)()
{
    uint32_t length = strnlen(currentDirectory, c_maxFilePathLength);
    if (length == 0)
    {
        return;
    }

    uint32_t position = length - 1;

    while (position > 0)
    {
        if (currentDirectory[position] == '/')
        {
            currentDirectory[position] = '\0';
            return;
        }
        
        currentDirectory[position] = '\0';
        position--;
    }

    currentDirectory[0] = '\0';
}

bool __time_critical_func(DirectoryListing::getDirectoryEntries)(const uint32_t offset)
{
    DIR dir;
    FILINFO currentEntry;
    FILINFO nextEntry;
    
    if (!sd_present)
	{
		fileListing->clear();
		fileListing->addString("No SD Card", 1);
		fileListing->addTerminator(0, 1);
		return true;
	}
    
    FRESULT res = f_opendir(&dir, currentDirectory);
    
    if (res != FR_OK)
    {
        DEBUG_PRINT("f_opendir error: (%d)\n", res);
        return false;
    }

    fileListing->clear();

    uint16_t fileEntryCount = 0;
    uint16_t filesProcessed = 0;
    bool hasNext = false;
	currentEntry.fname[0] = '\0';
	
    res = f_readdir(&dir, &currentEntry);
    if (res == FR_OK && currentEntry.fname[0] != '\0')
    {
        res = f_readdir(&dir, &nextEntry);
        hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        
        while (true)
        {
            if (isVisibleEntry(currentEntry))
            {
                if (filesProcessed >= offset)
                {
                    if (fileListing->addString(currentEntry.fname, currentEntry.fattrib & AM_DIR ? 1 : 0) == false)
                    {
                        hasNext = true;
                        break;
                    }
                    fileEntryCount++;
                }
                
                filesProcessed++;
                
                if (filesProcessed >= 4096)
                {
                    hasNext = 0;
                }
            }
            
            if (hasNext == 0)
            {
                break;
            }
            
            currentEntry = nextEntry;
            res = f_readdir(&dir, &nextEntry);
            hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        }
    }
    
    if (offset == 0)
    {
		uint16_t totalCount = getDirectoryEntriesCount();
		if (totalCount)
		{
			fileListing->addTerminator(hasNext ? 1 : 0, totalCount);
		}
		else
		{
			fileListing->addTerminator(0, 0xFEFE);
		}
        DEBUG_PRINT("file count: %d\n", totalCount);
    }
    else if (fileEntryCount)
    {
		fileListing->addTerminator(hasNext ? 1 : 0, fileEntryCount);
		DEBUG_PRINT("Entry count: %d\n", fileEntryCount);
    }
    else
	{
		DEBUG_PRINT("Empty directory\n");
		fileListing->addTerminator(0, 0xFEFE);
	}
    
    f_closedir(&dir);
    return true;
}

uint16_t __time_critical_func(DirectoryListing::getDirectoryEntriesCount)()
{
    DIR dir;
    FILINFO currentEntry;
    FILINFO nextEntry;
    
	if (!sd_present)
	{
		return 0;
	}
    
    FRESULT res = f_opendir(&dir, currentDirectory);
    
    if (res != FR_OK)
    {
        DEBUG_PRINT("f_opendir error: (%d)\n", res);
        return 0;
    }

    uint16_t fileEntryCount = 0;
    bool hasNext = false;

    res = f_readdir(&dir, &currentEntry);
    
    if (res == FR_OK && currentEntry.fname[0] != '\0')
    {
        res = f_readdir(&dir, &nextEntry);
        hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        
        while (true)
        {
            if (isVisibleEntry(currentEntry))
            {
				fileEntryCount++;
				
				if (fileEntryCount >= 4096)
				{
					hasNext = 0;
				}
            }
            
            if (hasNext == 0)
            {
                break;
            }
            
            currentEntry = nextEntry;
            res = f_readdir(&dir, &nextEntry);
            hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        }
    }
    
    f_closedir(&dir);
    return fileEntryCount;
}

uint16_t* __time_critical_func(DirectoryListing::getFileListingData)()
{
    return fileListing->getData();
}

// Private

bool __time_critical_func(DirectoryListing::combinePaths)(const char* directory, const char* name, char* newPath)
{
    const size_t directoryLength = strlen(directory);
    const size_t nameLength = strlen(name);
    const size_t separatorLength = directoryLength != 0;
    if (directoryLength + separatorLength + nameLength > c_maxFilePathLength)
    {
        return false;
    }

    if (directoryLength != 0)
    {
        memmove(newPath, directory, directoryLength);
        newPath[directoryLength] = '/';
    }
    memcpy(newPath + directoryLength + separatorLength, name, nameLength + 1);
    return true;
}

bool __time_critical_func(DirectoryListing::getDirectoryEntry)(uint32_t index, char* filePath, bool* isDirectory)
{
    if (index >= 4096)
    {
        return false;
    }

    DIR dir;
    FILINFO currentEntry;
    FILINFO nextEntry;
    FRESULT res = f_opendir(&dir, currentDirectory);
    
    if (res != FR_OK)
    {
        DEBUG_PRINT("f_opendir error: (%d)\n", res);
        return false;
    }

    uint32_t filesProcessed = 0;

    res = f_readdir(&dir, &currentEntry);
    
    if (res == FR_OK && currentEntry.fname[0] != '\0')
    {
        res = f_readdir(&dir, &nextEntry);
        bool hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        
        while (true)
        {
            if (isVisibleEntry(currentEntry))
            {
                if (filesProcessed == index)
                {
                    memcpy(filePath, currentEntry.fname, strlen(currentEntry.fname) + 1);
                    if (isDirectory)
                    {
                        *isDirectory = (currentEntry.fattrib & AM_DIR) != 0;
                    }
                    f_closedir(&dir);
                    return true;
                }
                
                filesProcessed++;
            }
            
            if (!hasNext)
            {
                break;
            }
            
            currentEntry = nextEntry;
            res = f_readdir(&dir, &nextEntry);
            hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        }
    }

    f_closedir(&dir);
    return false;
}

void __time_critical_func(DirectoryListing::openCover)(const uint32_t index)
{
    char filePath[c_maxFilePathLength + 1];
    
    if (cover_fp.obj.fs)
	{
		f_close(&cover_fp);
	}
	
    bool isDirectory;
    if (!sd_present || !getPath(index, filePath, &isDirectory))
    {
        return;
    }

    const size_t length = strlen(filePath);
    const size_t extensionOffset = isDirectory ? length : length - 4;
    if (extensionOffset + sizeof(".cov") > sizeof(filePath))
    {
        return;
    }
    memcpy(filePath + extensionOffset, ".cov", sizeof(".cov"));
    coverRgb15 = f_open(&cover_fp, filePath, FA_READ) == FR_OK &&
                 f_size(&cover_fp) == MENU_COVER_RGB15_BYTES;
}

uint16_t* __time_critical_func(DirectoryListing::readCover)(uint32_t part)
{
    static uint16_t coverBuffer[1162];
    UINT bytesRead = 0;

    const size_t coverSize = coverRgb15 ? MENU_COVER_RGB15_BYTES : MENU_COVER_BYTES;
    const size_t partSize = coverRgb15 ? MENU_COVER_RGB15_PART_BYTES : MENU_COVER_PART_BYTES;
    const size_t partCount = coverRgb15 ? MENU_COVER_RGB15_PARTS : MENU_COVER_PARTS;
    if (cover_fp.obj.fs && part < partCount)
    {
        const size_t offset = part * partSize;
        const size_t remaining = coverSize - offset;
        const UINT length = remaining < partSize ? remaining : partSize;
        auto* data = reinterpret_cast<uint8_t*>(coverBuffer);
        auto* payload = data + MENU_COVER_DATA_OFFSET;
        if (f_lseek(&cover_fp, offset) == FR_OK && f_tell(&cover_fp) == offset &&
            f_read(&cover_fp, payload, length, &bytesRead) == FR_OK && bytesRead == length)
        {
            memset(payload + length, 0, sizeof(coverBuffer) - MENU_COVER_DATA_OFFSET - length);
            if (coverRgb15)
                memcpy(data + MENU_COVER_FORMAT_OFFSET, "RGB5", 4);
            coverBuffer[0] = 'P' | 'A' << 8;
            coverBuffer[1] = 'R' | 'T' << 8;
            coverBuffer[2] = static_cast<uint16_t>(part);
            return coverBuffer;
        }
    }

    coverBuffer[0] = 'D' | 'E' << 8;
    coverBuffer[1] = 'A' | 'D' << 8;
    return coverBuffer;
}

void __time_critical_func(DirectoryListing::openCfg)(void)
{
    if (cfg_fp.obj.fs)
	{
		f_close(&cfg_fp);
	}
	
    if (!sd_present)
    {
		return;
	}
	
	DEBUG_PRINT("openCfg\n");

	f_open(&cfg_fp, "config.ini", FA_READ);
}

uint16_t* __time_critical_func(DirectoryListing::readCfg)(void)
{
	static uint16_t cfg_buf[1162];
	UINT bytesRead = 0;
	
	memset(cfg_buf, 0, sizeof(cfg_buf));
	
	if (!cfg_fp.obj.fs || f_lseek(&cfg_fp, 0) != FR_OK ||
        f_read(&cfg_fp, &cfg_buf[MENU_CONFIG_DATA_OFFSET / sizeof(uint16_t)], MENU_CONFIG_MAX_BYTES,
               &bytesRead) != FR_OK)
	{
		DEBUG_PRINT("readCfg: not found\n");
		cfg_buf[0] = 'D' | 'E' << 8;
		cfg_buf[1] = 'A' | 'D' << 8;
		return cfg_buf;
	}
	
	cfg_buf[0] = 'C' | 'F' << 8;
	cfg_buf[1] = 'G' | '1' << 8;
	cfg_buf[2] = static_cast<uint16_t>(bytesRead);
	
	DEBUG_PRINT("readCfg: read %u bytes\n", bytesRead);
	
	return cfg_buf;
}

}  // namespace picostation

