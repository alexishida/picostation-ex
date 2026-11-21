#pragma once

#include <stdint.h>

#include <array>

#include "pseudo_atomics.h"
#include "hardware/dma.h"
#include "pico/critical_section.h"
#include "ff.h"
#include "disc_image.h"

#define CACHED_SECS		32 /* Only 2, 4, 8, 16, 32 */

namespace picostation {
class MechCommand;

class I2S {
  public:
    I2S() {};
    int dmaChannel;
    void initTransferLock() { critical_section_init(&transferLock); }
    void i2s_set_state(uint8_t state);
    void publishDriveSector(int sector);
    int getDriveSector() { return transferSnapshot().sector; }
    void setReadOption(DiscImage::ReadOption option, bool enabled);
    void notifyDoor();

    int getSectorSending() { return m_sectorSending.Load(); }
    uint64_t getLastSectorTime() { return m_lastSectorTime.Load(); }
    void resetFromConsole(bool returnToMenu);

    [[noreturn]] void start(MechCommand &mechCommand);
	
  private:
    int initDMA(const volatile void *read_addr, unsigned int transfer_count);  // Returns DMA channel number
    void reinitI2S();
    enum class ResetRequest : uint8_t { None, RestartDisc, ReturnToMenu };
    pseudoatomic<ResetRequest> resetRequest;
    bool menu_active;
	
	int loadedSector[CACHED_SECS];
	int lastSector;
    struct TransferState {
        int sector = 0;
        uint32_t generation = 0;
        uint8_t options = 0;
        bool enabled = false;
    } transfer;
    critical_section_t transferLock;
    TransferState transferSnapshot();
    enum class TransferResult { Started, Stale, ClockTimeout };
    TransferResult startSectorDMA(const TransferState& expected);
    bool doorEnabled = false;
    bool doorPending = false;
    void enableDoor(bool enabled);
    bool takeDoorEvent();
	
    pseudoatomic<int> m_sectorSending;
    pseudoatomic<uint64_t> m_lastSectorTime;
};
}  // namespace picostation
