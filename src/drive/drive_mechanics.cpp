#include "drive_mechanics.h"

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include "i2s.h"
#include "cmd.h"
#include "values.h"
#include "logging.h"

#if DEBUG_CMD
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...) while (0)
#endif

#define ZONE_CNT 	16
#define ZONE_MAX 	ZONE_CNT-1

extern picostation::I2S m_i2s;
uint32_t zone[ZONE_CNT] = 			{4500, 7750, 13500, 27000, 45000, 63000, 85500, 103500, 130500, 153000, 175500, 207000, 234000, 265500, 297000, 999999};
uint32_t sect_per_track[ZONE_CNT] = {	8,    9,    10,    11,    12,    13,    14,     15,     16,     17,     18,     19,     20,     21,     22,     23};

picostation::DriveMechanics picostation::g_driveMechanics;

int picostation::DriveMechanics::getSector() { return m_i2s.getDriveSector(); }

void picostation::DriveMechanics::resetDrive()
{
    sled_work = false;
    m_sector = 0;
    cur_track_counter = 0;
    cur_zone = 0;
    skip_subq = 0;
    m_i2s.publishDriveSector(0);
}

void __time_critical_func(picostation::DriveMechanics::moveToNextSector)()
{
	const uint32_t irqState = save_and_disable_interrupts();
	if(m_sector < c_sectorMax.Load()){
		m_sector++;
        m_i2s.publishDriveSector(m_sector);
	}
	
	if (m_sector > zone[cur_zone] && cur_zone < ZONE_MAX)
	{
		cur_zone++;
	}
    restore_interrupts(irqState);
}

void __time_critical_func(picostation::DriveMechanics::setSector)(uint32_t step, bool rev)
{
	m_i2s.i2s_set_state(0);

    const uint32_t sectorLimit = c_sectorMax.Load();
    m_sector = std::min(m_sector, sectorLimit);
    cur_zone = std::lower_bound(zone, zone + ZONE_MAX, m_sector) - zone;

    while (step != 0)
    {
        if (!rev)
        {
            const uint32_t limit = cur_zone == ZONE_MAX ? sectorLimit :
                                   std::min(zone[cur_zone], sectorLimit);
            const uint32_t sectorsPerTrack = sect_per_track[cur_zone];
            const uint32_t steps = std::min(step, (limit - m_sector) / sectorsPerTrack);
            m_sector += steps * sectorsPerTrack;
            step -= steps;
            if (step == 0)
                break;
            if (limit == sectorLimit)
            {
                m_sector = sectorLimit;
                break;
            }
            ++cur_zone;
        }
        else
        {
            const uint32_t sectorsPerTrack = sect_per_track[cur_zone];
            const uint32_t lower = cur_zone == 0 ? 0 : zone[cur_zone - 1];
            const uint32_t steps = std::min(step, (m_sector - lower) / sectorsPerTrack);
            m_sector -= steps * sectorsPerTrack;
            step -= steps;
            if (step == 0)
                break;
            if (cur_zone == 0)
            {
                m_sector = 0;
                break;
            }
            --cur_zone;
        }
	}
	
    cur_zone = std::lower_bound(zone, zone + ZONE_MAX, m_sector) - zone;
    skip_subq = m_sector;
    m_i2s.publishDriveSector(m_sector);
#ifdef DEBUG_CMD	
	DEBUG_PRINT("set sector %d\n", m_sector-4500);
#endif
}

bool __time_critical_func(picostation::DriveMechanics::servo_valid)()
{
	return m_sector < c_sectorMax.Load();

}

void __time_critical_func(picostation::DriveMechanics::moveSled)(picostation::MechCommand &mechCommand){
    if ((time_us_64() - m_sledTimer) > c_MaxTrackMoveTime)
    {
		cur_track_counter++;
		
		if (!(cur_track_counter & 255))
		{
			mechCommand.setSens(SENS::COUT, !mechCommand.getSens(SENS::COUT));
		}
		
		m_sledTimer = time_us_64();
    }
}

void __time_critical_func(picostation::DriveMechanics::startSled)()
{
	sled_work = true;
	cur_track_counter = 0;
	m_sledTimer = time_us_64();
}

