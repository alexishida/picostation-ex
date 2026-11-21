#pragma once

#include <algorithm>
#include <stdint.h>

#include "pseudo_atomics.h"
#include "values.h"

namespace picostation {
class MechCommand;

class DriveMechanics {
  public:
    void moveToNextSector();
    void setSector(uint32_t step, bool rev);
    int getSector();
    void moveSled(MechCommand &mechCommand);
    bool servo_valid();
    void startSled();
    void stopSled() { sled_work = false; }
    uint32_t get_track_count() { return cur_track_counter;}
	
    void resetDrive();

    bool isSledStopped() { return !sled_work; }
    
    uint32_t req_skip_subq() { return skip_subq; }
    void clear_skip_subq() { skip_subq = 0; }
    
  private:
	uint32_t cur_track_counter = 0;
    uint64_t m_sledTimer = 0;
    uint32_t m_sector = 0;
    uint8_t cur_zone = 0;
#ifdef PICOSTATION_SLED_STEP_US
    uint32_t c_MaxTrackMoveTime = PICOSTATION_SLED_STEP_US;
#else
    uint32_t c_MaxTrackMoveTime = 29;
#endif
    bool sled_work = false;
    uint32_t skip_subq = 0;
};

extern DriveMechanics g_driveMechanics;
}  // namespace picostation

