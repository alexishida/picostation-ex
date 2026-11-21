#pragma once

#include <stdint.h>

#include "../third_party/cueparser/cueparser.h"
#include "../third_party/cueparser/disc.h"
#include "../third_party/cueparser/scheduler.h"
#include "../third_party/posix_file.h"
#include "ff.h"
#include "subq.h"

namespace picostation {
class DiscImage {
  public:
    DiscImage() {};
    ~DiscImage() {};

    enum DataLocation {
        RAM,
        SDCard
    };

    void buildSector(const int sector, uint32_t *buffer, uint16_t *userData, const uint16_t *scramling, bool pregap = false);
    FRESULT load(const TCHAR *targetCue, bool updateSectorLimit = true);
    int sectorLimit() const;
    void unload();
    void replacePreparedImage(DiscImage& prepared, DiscImage& previous);
    SubQ::Data generateSubQ(const int sector);
    bool hasData() { return m_hasData; };
    void makeDummyCue(bool updateSectorLimit = true);
    enum ReadOption : uint8_t { SkipBootSector = 1, SkipBootSectorFix = 2, SkipEdc = 4 };
    bool readSector(void *buffer, int sector, DataLocation location, const uint16_t *scramling, uint8_t options);
    void readSectorRAM(void *buffer, int sector, const uint16_t *scramling);
    bool readSectorSD(void *buffer, int sector, const uint16_t *scramling, uint8_t options);

  private:
    CueDisc m_cueDisc{};
    bool m_hasData = false;
    int m_currentLogicalTrack = 0;
    uint64_t m_nextReadRecovery = 0;
};

extern DiscImage g_discImage;
}  // namespace picostation
