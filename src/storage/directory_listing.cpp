#include "directory_listing.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "hardware/timer.h"
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

static bool appendLog(const char* line)
{
    static FIL log;
    if (sd_present && f_open(&log, "/erro.log", FA_OPEN_APPEND | FA_WRITE) == FR_OK)
    {
        UINT written;
        const UINT size = strlen(line);
        const FRESULT result = f_write(&log, line, size, &written);
        const FRESULT closed = f_close(&log);
        return result == FR_OK && written == size && closed == FR_OK;
    }
    return false;
}

#ifdef PICOSTATION_SD_BOOT_LOG
void DirectoryListing::appendBootLog(const char* line)
{
    appendLog(line);
}
#endif

namespace {
struct ErrorEntry {
    uint64_t firstUs;
    uint64_t nextWrite;
    unsigned count;
    int detail;
};
ErrorEntry errors[static_cast<unsigned>(DirectoryListing::Error::Count)];
unsigned pendingErrors;
}

// Called only by the sector reader on core 1. Keep disk I/O out of the LRCK wait.
void DirectoryListing::recordError(Error error, int detail)
{
    auto& entry = errors[static_cast<unsigned>(error)];
    if (!entry.count)
    {
        entry.firstUs = time_us_64();
        entry.detail = detail;
        ++pendingErrors;
    }
    if (entry.count < UINT32_MAX)
        ++entry.count;
}

void __time_critical_func(DirectoryListing::flushErrors)()
{
    if (!pendingErrors || !sd_present)
        return;

    static constexpr const char* names[] = {
        "montar_sd", "listar_diretorio", "preparar_imagem", "montar_imagem",
        "ler_setor", "timeout_lrck", "ler_capa", "ler_config"
    };
    static_assert(sizeof(names) / sizeof(names[0]) == sizeof(errors) / sizeof(errors[0]));
    const uint64_t now = time_us_64();
    for (unsigned i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
    {
        auto& entry = errors[i];
        if (!entry.count || now < entry.nextWrite)
            continue;
        char line[128];
        snprintf(line, sizeof(line), "ERRO ms=%llu evento=%s detalhe=%d ocorrencias=%u\r\n",
                 (unsigned long long)(entry.firstUs / 1000), names[i], entry.detail, entry.count);
        entry.nextWrite = now + 5000000;
        if (appendLog(line))
        {
            entry.count = 0;
            --pendingErrors;
        }
        break;
    }
}

void __time_critical_func(DirectoryListing::init)()
{
    sd_present = false;
    
    memset(&cover_fp, 0, sizeof(FIL));
    memset(&cfg_fp, 0, sizeof(FIL));
    
    FRESULT fr = f_mount(&s_fatFS, "", 1);
	
    if (fr != FR_OK)
        recordError(Error::MountSD, fr);
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
    char newPath[c_maxFilePathLength + 1];
    if (!getDirectoryEntry(index, newFolder, &isDirectory) || !isDirectory ||
        !combinePaths(currentDirectory, newFolder, newPath))
        return false;

    DIR dir;
    if (f_opendir(&dir, newPath) != FR_OK)
        return false;
    f_closedir(&dir);
    strcpy(currentDirectory, newPath);
    DEBUG_PRINT("gotoDirectory: %s\n", currentDirectory);
    return true;
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

void __time_critical_func(DirectoryListing::setListingError)(MenuListingStatus status)
{
    fileListing->clear();
    fileListing->addTerminator(status, 0);
    recordError(Error::Listing, status);
}

bool __time_critical_func(DirectoryListing::getDirectoryEntries)(const uint32_t offset)
{
    DIR dir;
    FILINFO entry;

    if (!sd_present)
    {
        sd_present = f_mount(&s_fatFS, "", 1) == FR_OK;
        if (!sd_present)
        {
            setListingError(MENU_LIST_NO_SD);
            return false;
        }
    }

    FRESULT result = f_opendir(&dir, currentDirectory);
    if (result != FR_OK)
    {
        sd_present = result != FR_NOT_READY && result != FR_NOT_ENABLED;
        setListingError(sd_present ? MENU_LIST_READ_FAILED : MENU_LIST_NO_SD);
        return false;
    }

    fileListing->clear();
    uint16_t visibleCount = 0;
    uint16_t pageCount = 0;
    bool hasNext = false;

    while (visibleCount < 4096)
    {
        result = f_readdir(&dir, &entry);
        if (result != FR_OK || !entry.fname[0])
            break;
        if (!isVisibleEntry(entry))
            continue;

        if (visibleCount++ >= offset && !hasNext)
        {
            if (fileListing->addString(entry.fname, (entry.fattrib & AM_DIR) != 0))
                ++pageCount;
            else
            {
                hasNext = true;
                if (offset != 0)
                    break;
            }
        }
    }

    f_closedir(&dir);
    if (result != FR_OK)
    {
        sd_present = result != FR_NOT_READY && result != FR_NOT_ENABLED;
        setListingError(sd_present ? MENU_LIST_READ_FAILED : MENU_LIST_NO_SD);
        return false;
    }

    const uint16_t count = offset ? pageCount : visibleCount;
    fileListing->addTerminator(hasNext ? MENU_LIST_MORE : MENU_LIST_DONE, count ? count : 0xFEFE);
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
    const FRESULT result = f_open(&cover_fp, filePath, FA_READ);
    if (result != FR_OK && result != FR_NO_FILE && result != FR_NO_PATH)
        recordError(Error::Cover, index);
    coverRgb15 = result == FR_OK && f_size(&cover_fp) == MENU_COVER_RGB15_BYTES;
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
        recordError(Error::Cover, part);
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

    const FRESULT result = f_open(&cfg_fp, "config.ini", FA_READ);
    if (result != FR_OK && result != FR_NO_FILE && result != FR_NO_PATH)
        recordError(Error::Config, result);
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
        if (cfg_fp.obj.fs)
            recordError(Error::Config, f_error(&cfg_fp));
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

