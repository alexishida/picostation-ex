#pragma once

#include <stddef.h>
#include <stdint.h>
#include "menu_protocol.h"

namespace picostation {
class DirectoryListing {
  public:
    
	static void init();
#ifdef PICOSTATION_SD_BOOT_LOG
    static void appendBootLog(const char* line);
#endif
	static void gotoRoot();
    enum class Error { MountSD, Listing, Prepare, MountImage, SectorRead, LRCK, Cover, Config, DirectoryIO, MenuListTransfer, MenuListPage, MenuConfig, MenuCover, MenuPrepare, MenuDisc, Count };
    static void recordError(Error error, int detail);
    static void reportMenuError(unsigned code);
    static void logDiscEvent(const char* event, const char* path = nullptr);
    static void beginMenuLog();
    static void flushErrors();

    static bool gotoDirectory(const uint32_t index);
    static bool getPath(const uint32_t index, char* filePath, bool* isDirectory = nullptr);
	static void gotoParentDirectory();
    static bool getDirectoryEntries(const uint32_t offset);
    static void setListingError(MenuListingStatus status);
	static uint16_t getDirectoryEntriesCount();
	static uint16_t* getFileListingData();
    static void openCover(const uint32_t index);
    static uint16_t *readCover(const uint32_t part);
    static void openCfg(void);
    static uint16_t *readCfg(void);
  private:
	static bool combinePaths(const char* filePath1, const char* filePath2, char* newPath);
	static bool getDirectoryEntry(const uint32_t index, char* filePath, bool* isDirectory = nullptr);
};
}  // namespace picostation

