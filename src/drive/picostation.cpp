#include "picostation.h"

#include <stdio.h>
#include <time.h>

#include "cmd.h"
#include "disc_image.h"
#include "drive_mechanics.h"
#include "hardware/pwm.h"
#include <hardware/i2c.h>
#include "i2s.h"
#include "logging.h"
#include "main.pio.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pseudo_atomics.h"
#include "subq.h"
#include "values.h"
#include "si5351.h"

#if DEBUG_MAIN
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while (0)
#endif

// To-do: Implement lid switch behavior
// To-do: Implement a console side menu to select the cue file
// To-do: Implement level meter mode to command $AX - AudioCTRL
// To-do: Fix seeks that go into the lead-in + track 1 pregap areas, possibly sending bad data over I2S

// To-do: Make an ODE class and move these to members
picostation::I2S m_i2s;
picostation::MechCommand m_mechCommand;

pseudoatomic<bool> picostation::g_subqDelay;
static volatile alarm_id_t s_subqAlarm;
static pseudoatomic<bool> s_discPauseRequested;
static pseudoatomic<bool> s_discPauseAcknowledged;

static int s_currentPlaybackSpeed = 1;
int picostation::g_targetPlaybackSpeed = 1;  // core0: r/w

mutex_t picostation::g_mechaconMutex;
pseudoatomic<bool> picostation::g_coreReady[2];

unsigned int picostation::g_audioCtrlMode = audioControlModes::NORMAL;

pseudoatomic<picostation::FileListingStates> picostation::g_fileListingState;
pseudoatomic<uint32_t> picostation::g_fileArg;

static unsigned int s_mechachonOffset;
unsigned int picostation::g_soctOffset;
unsigned int picostation::g_subqOffset;

static uint8_t s_resetPending = 0;

static picostation::PWMSettings pwmDataClock = 
{
	.gpio = Pin::DA15,
	.wrap = (1 * 32) - 1, 
	.clkdiv = 4, 
	.invert = true, 
	.level = (32 / 2)
};

static picostation::PWMSettings pwmLRClock = 
{
	.gpio = Pin::LRCK,
	.wrap = (48 * 32) - 1,
	.clkdiv = 4,
	.invert = false,
	.level = (48 * (32 / 2))
};

static picostation::PWMSettings pwmMainClock =
{
	.gpio = Pin::CLK,
	.wrap = 1,
	.clkdiv = 2,
	.invert = false,
	.level = 1
};

#ifdef GPIO_MECHACON_CLK
static picostation::PWMSettings pwmMechaconClock = {
    .gpio = Pin::MECHACON_CLK,
    .wrap = PICOSTATION_MECHACON_CLOCK_PERIOD - 1,
    .clkdiv = 1,
    .invert = false,
    .level = PICOSTATION_MECHACON_CLOCK_PERIOD / 2
};
static uint32_t s_mechaconClockMask;
#else
static constexpr uint32_t s_mechaconClockMask = 0;
#endif

static void initPWM(picostation::PWMSettings *settings);

static void __time_critical_func(interruptHandler)(unsigned int gpio, uint32_t events)
{
    static uint64_t lastLowEvent = 0;
	static uint64_t lastLowEventDoor = 0;

    switch (gpio)
    {
        case Pin::RESET:
        {
            if (events & GPIO_IRQ_LEVEL_LOW)
            {
                lastLowEvent = time_us_64();
                // Disable low signal edge detection
                gpio_set_irq_enabled(Pin::RESET, GPIO_IRQ_LEVEL_LOW, false);
                // Enable high signal edge detection
                gpio_set_irq_enabled(Pin::RESET, GPIO_IRQ_LEVEL_HIGH, true);
            } 
            else if (events & GPIO_IRQ_LEVEL_HIGH)
            {
                // Disable the rising edge detection
                gpio_set_irq_enabled(Pin::RESET, GPIO_IRQ_LEVEL_HIGH, false);

                const uint64_t c_now = time_us_64();
                const uint64_t c_timeElapsed = c_now - lastLowEvent;
                
                if (c_timeElapsed >= 50000U)  // Debounce, only reset if the pin was low for more than 50000us(50 ms)
                {
                    if (c_timeElapsed >= 1000000U) // pressed more one second
					{
						s_resetPending = 2;
					}
					else
					{
						s_resetPending = 1;
					}
                }
                
				// Enable the low signal edge detection again
				gpio_set_irq_enabled(Pin::RESET, GPIO_IRQ_LEVEL_LOW, true);
            }
        } break;
        
        case Pin::DOOR:
        {
            if (events & GPIO_IRQ_LEVEL_HIGH)
            {
                lastLowEventDoor = time_us_64();
                m_i2s.notifyDoorOpened();
                // Disable low signal edge detection
                gpio_set_irq_enabled(Pin::DOOR, GPIO_IRQ_LEVEL_HIGH, false);
                // Enable high signal edge detection
                gpio_set_irq_enabled(Pin::DOOR, GPIO_IRQ_LEVEL_LOW, true);
            }
            else if (events & GPIO_IRQ_LEVEL_LOW)
            {
                // Disable the rising edge detection
                gpio_set_irq_enabled(Pin::DOOR, GPIO_IRQ_LEVEL_LOW, false);

                const uint64_t c_now = time_us_64();
                const uint64_t c_timeElapsed = c_now - lastLowEventDoor;
                if (c_timeElapsed >= 50000U)  // Debounce, only reset if the pin was low for more than 50000us(50 ms)
                {
                    m_i2s.notifyDoor();
                }
                
                // Enable the low signal edge detection again
                gpio_set_irq_enabled(Pin::DOOR, GPIO_IRQ_LEVEL_HIGH, true);
            }
        } break;

        case Pin::XLAT:
        {
            m_mechCommand.processLatchedCommand();
        } break;
    }
}

static void __time_critical_func(mech_irq_hnd)()
{
	// Update latching
	m_mechCommand.updateMech();
	pio_interrupt_clear(PIOInstance::MECHACON, 0);
}

static void __time_critical_func(send_subq)(const int Sector)
{
    s_subqAlarm = 0;
	picostation::SubQ subq(&picostation::g_discImage);
	
	subq.start_subq(Sector);
	picostation::g_subqDelay = false;
}

void picostation::requestDiscPause()
{
    s_discPauseRequested = true;
    while (!s_discPauseAcknowledged.Load())
        tight_loop_contents();
}

void picostation::releaseDiscPause()
{
    s_discPauseRequested = false;
    while (s_discPauseAcknowledged.Load())
        tight_loop_contents();
}

void __time_critical_func(picostation::serviceDiscPause)(bool resetting)
{
    if (!s_discPauseRequested.Load())
        return;

    gpio_set_irq_enabled(Pin::XLAT, GPIO_IRQ_EDGE_FALL, false);
    irq_set_enabled(PIO0_IRQ_0, false);
    const alarm_id_t pendingAlarm = s_subqAlarm;
    s_subqAlarm = 0;
    if (pendingAlarm > 0)
        cancel_alarm(pendingAlarm);
    pio_sm_set_enabled(PIOInstance::SUBQ, SM::SUBQ, false);
    pio_sm_clear_fifos(PIOInstance::SUBQ, SM::SUBQ);
    gpio_put(Pin::SCOR, 0);
    gpio_put(Pin::SQSO, 0);
    g_subqDelay = false;
    s_discPauseAcknowledged = true;
    while (s_discPauseRequested.Load())
        tight_loop_contents();
    g_driveMechanics.resetDrive();
    irq_set_enabled(PIO0_IRQ_0, true);
    if (!resetting)
        gpio_set_irq_enabled(Pin::XLAT, GPIO_IRQ_EDGE_FALL, true);
    s_discPauseAcknowledged = false;
}

[[noreturn]] void __time_critical_func(picostation::core0Entry)()
{
    g_coreReady[0] = true;
    while (!g_coreReady[1].Load())
    {
        tight_loop_contents();
    }

    while (true)
    {
        serviceDiscPause(false);
        if (s_resetPending)
        {
            while (gpio_get(Pin::RESET) == 0)
            {
                serviceDiscPause(false);
            }
			reset();
		}

        const int currentSector = g_driveMechanics.getSector();

        // Limit Switch
        gpio_put(Pin::LMTSW, currentSector > 3000);

        updatePlaybackSpeed();

        // Soct/Sled/seek
        if (m_mechCommand.getSoct())
        {
            if (pio_sm_get_rx_fifo_level(PIOInstance::SOCT, SM::SOCT))
            {
                pio_sm_drain_tx_fifo(PIOInstance::SOCT, SM::SOCT);
                m_mechCommand.setSoct(false);
                pio_sm_set_enabled(PIOInstance::SOCT, SM::SOCT, false);
            }
        }
        else if (!g_driveMechanics.isSledStopped())
        {
            g_driveMechanics.moveSled(m_mechCommand);
        }
        else if (m_mechCommand.getSens(SENS::GFS))
        {
            if (m_i2s.getSectorSending() == currentSector && !g_subqDelay.Load())
            {
                g_driveMechanics.moveToNextSector();
                g_subqDelay = true;

                const uint64_t subqDelay = time_us_64() - m_i2s.getLastSectorTime() + c_MaxSubqDelayTime;
                const uint32_t interruptState = save_and_disable_interrupts();
                s_subqAlarm = add_alarm_in_us(subqDelay,
					[](alarm_id_t id, void *user_data) -> int64_t {
                        if (id == s_subqAlarm)
						    send_subq((const int) user_data);
						return 0;
					}, (void *) currentSector, true);
                restore_interrupts(interruptState);
                if (s_subqAlarm < 0)
                    g_subqDelay = false;
            }
        }
    }
}

[[noreturn]] void picostation::core1Entry()
{
    m_i2s.start(m_mechCommand);
    while (1) asm("");
    __builtin_unreachable();
}

void __time_critical_func(picostation::initHW)()
{
    m_i2s.initTransferLock();
#if DEBUG_LOGGING_ENABLED
	stdio_init_all();
    stdio_set_chars_available_callback(NULL, NULL);
    sleep_ms(1250);
#endif

    DEBUG_PRINT("Initializing...\n");

    mutex_init(&g_mechaconMutex);

    for (const unsigned int pin : Pin::allPins)
    {
        gpio_init(pin);
    }
    
    gpio_put(Pin::SD_CS, 1);
    gpio_set_dir(Pin::SD_CS, GPIO_OUT);
    
    gpio_set_dir(Pin::XLAT, GPIO_IN);
    gpio_set_dir(Pin::SQCK, GPIO_IN);
    gpio_set_dir(Pin::LMTSW, GPIO_OUT);
    gpio_set_dir(Pin::DOOR, GPIO_IN);
    gpio_set_dir(Pin::RESET, GPIO_IN);
    gpio_set_dir(Pin::SENS, GPIO_OUT);
    gpio_set_dir(Pin::DA15, GPIO_OUT);
    gpio_set_dir(Pin::DA16, GPIO_OUT);
    gpio_set_dir(Pin::LRCK, GPIO_OUT);
    gpio_set_dir(Pin::SCOR, GPIO_OUT);
    gpio_put(Pin::SCOR, 0);
    gpio_set_dir(Pin::SQSO, GPIO_OUT);
    gpio_put(Pin::SQSO, 0);
    gpio_set_dir(Pin::CLK, GPIO_OUT);
    gpio_set_dir(Pin::CMD_CK, GPIO_IN);
    gpio_set_dir(Pin::CMD_DATA, GPIO_IN);

    gpio_set_input_hysteresis_enabled(Pin::XLAT, true);
    gpio_set_input_hysteresis_enabled(Pin::SQCK, true);
    gpio_set_input_hysteresis_enabled(Pin::RESET, true);
    gpio_set_input_hysteresis_enabled(Pin::CMD_CK, true);
    
    i2c_init(i2c0, 400*1000);
	gpio_set_function(Pin::EXP_I2C0_SDA, GPIO_FUNC_I2C);
	gpio_set_function(Pin::EXP_I2C0_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(Pin::EXP_I2C0_SDA);
	gpio_pull_up(Pin::EXP_I2C0_SCL);
	
	// Initialize the Si5351
	if (si5351_Init(0))
	{
		si5351_SetupCLK1(53203425, SI5351_DRIVE_STRENGTH_8MA);
		si5351_SetupCLK2(53693175, SI5351_DRIVE_STRENGTH_8MA);
		si5351_EnableOutputs((1<<1) | (1<<2));
	}

    initPWM(&pwmMainClock);
    initPWM(&pwmDataClock);
    initPWM(&pwmLRClock);
#ifdef GPIO_MECHACON_CLK
    initPWM(&pwmMechaconClock);
    s_mechaconClockMask = 1u << pwmMechaconClock.sliceNum;
#endif

    uint32_t i2s_pio_offset = pio_add_program(PIOInstance::I2S_DATA, &i2s_data_program);
    i2s_data_program_init(PIOInstance::I2S_DATA, SM::I2S_DATA, i2s_pio_offset, Pin::DA15, Pin::DA16);

    s_mechachonOffset = pio_add_program(PIOInstance::MECHACON, &mechacon_program);
    mechacon_program_init(PIOInstance::MECHACON, SM::MECHACON, s_mechachonOffset, Pin::CMD_DATA);

    g_soctOffset = pio_add_program(PIOInstance::SOCT, &soct_program);
    g_subqOffset = pio_add_program(PIOInstance::SUBQ, &subq_program);

    pio_sm_set_enabled(PIOInstance::I2S_DATA, SM::I2S_DATA, true);
    pwm_set_mask_enabled(s_mechaconClockMask | (1u << pwmLRClock.sliceNum) |
                         (1u << pwmDataClock.sliceNum) | (1u << pwmMainClock.sliceNum));

    uint64_t startTime = time_us_64();
    gpio_set_dir(Pin::RESET, GPIO_OUT);
    gpio_put(Pin::RESET, 0);
    sleep_ms(300);
    gpio_set_dir(Pin::RESET, GPIO_IN);

    while ((time_us_64() - startTime) < 30000)
    {
        if (gpio_get(Pin::RESET) == 0)
        {
            startTime = time_us_64();
        }
    }

    while ((time_us_64() - startTime) < 30000)
    {
        if (gpio_get(Pin::CMD_CK) == 0)
        {
            startTime = time_us_64();
        }
    }

    gpio_set_irq_enabled_with_callback(Pin::RESET, GPIO_IRQ_LEVEL_LOW, true, &interruptHandler);
    gpio_set_irq_enabled_with_callback(Pin::DOOR, GPIO_IRQ_LEVEL_HIGH, true, &interruptHandler);
    gpio_set_irq_enabled_with_callback(Pin::XLAT, GPIO_IRQ_EDGE_FALL, true, &interruptHandler);

    pio_sm_set_enabled(PIOInstance::MECHACON, SM::MECHACON, true);
    
    pio_set_irq0_source_enabled(PIOInstance::MECHACON, (enum pio_interrupt_source)pis_interrupt0, true);
    
    
    pio_interrupt_clear(PIOInstance::MECHACON, 0);
    irq_set_exclusive_handler(PIO0_IRQ_0, mech_irq_hnd);
    irq_set_enabled(PIO0_IRQ_0, true);

    g_coreReady[0] = false;
    g_coreReady[1] = false;
    s_discPauseRequested = false;
    s_discPauseAcknowledged = false;

    DEBUG_PRINT("ON!\n");
}

static void __time_critical_func(initPWM)(picostation::PWMSettings *settings)
{
    gpio_set_function(settings->gpio, GPIO_FUNC_PWM);
    settings->sliceNum = pwm_gpio_to_slice_num(settings->gpio);
    settings->config = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&settings->config, PWM_DIV_FREE_RUNNING);
    pwm_config_set_wrap(&settings->config, settings->wrap);
    pwm_config_set_clkdiv(&settings->config, settings->clkdiv);
    pwm_config_set_output_polarity(&settings->config, settings->invert, settings->invert);
    pwm_init(settings->sliceNum, &settings->config, false);
    pwm_set_both_levels(settings->sliceNum, settings->level, settings->level);
}

void __time_critical_func(picostation::updatePlaybackSpeed)()
{
    static constexpr unsigned int c_clockDivNormal = 4;
    static constexpr unsigned int c_clockDivDouble = 2;

    if (s_currentPlaybackSpeed != g_targetPlaybackSpeed)
    {
        s_currentPlaybackSpeed = g_targetPlaybackSpeed;
        const unsigned int clock_div = (s_currentPlaybackSpeed == 1) ? c_clockDivNormal : c_clockDivDouble;
        pwm_set_mask_enabled(s_mechaconClockMask);
        pwm_config_set_clkdiv_int(&pwmDataClock.config, clock_div);
        pwm_config_set_clkdiv_int(&pwmLRClock.config, clock_div);
        pwm_hw->slice[pwmDataClock.sliceNum].div = pwmDataClock.config.div;
        pwm_hw->slice[pwmLRClock.sliceNum].div = pwmLRClock.config.div;
        pwm_set_mask_enabled(s_mechaconClockMask | (1u << pwmLRClock.sliceNum) |
                             (1u << pwmDataClock.sliceNum) | (1u << pwmMainClock.sliceNum));
        DEBUG_PRINT("x%i\n", s_currentPlaybackSpeed);
    }
}

void __time_critical_func(picostation::reset)()
{
    DEBUG_PRINT("RESET!\n");
    m_i2s.i2s_set_state(0);
    gpio_set_irq_enabled(Pin::XLAT, GPIO_IRQ_EDGE_FALL, false);
    const alarm_id_t pendingAlarm = s_subqAlarm;
    s_subqAlarm = 0;
    if (pendingAlarm > 0)
        cancel_alarm(pendingAlarm);
    pio_sm_set_enabled(PIOInstance::SUBQ, SM::SUBQ, false);
    pio_sm_set_enabled(PIOInstance::SOCT, SM::SOCT, false);
    pio_sm_restart(PIOInstance::MECHACON, SM::MECHACON);

    pio_sm_clear_fifos(PIOInstance::MECHACON, SM::MECHACON);
    pio_sm_clear_fifos(PIOInstance::SOCT, SM::SOCT);
    pio_sm_clear_fifos(PIOInstance::SUBQ, SM::SUBQ);

    g_targetPlaybackSpeed = 1;
    updatePlaybackSpeed();
    
    m_i2s.resetFromConsole(s_resetPending == 2);

    mechacon_program_init(PIOInstance::MECHACON, SM::MECHACON, s_mechachonOffset, Pin::CMD_DATA);
    g_subqDelay = false;
    m_mechCommand.setSoct(false);

    gpio_put(Pin::SCOR, 0);
    gpio_put(Pin::SQSO, 0);
	g_driveMechanics.resetDrive();
    m_mechCommand.resetBootSectorPattern();
	
	uint64_t startTime = time_us_64();
	
	while ((time_us_64() - startTime) < 30000)
	{
		if (gpio_get(Pin::RESET) == 0)
		{
			startTime = time_us_64();
		}
	}
	
	while ((time_us_64() - startTime) < 30000)
	{
		if (gpio_get(Pin::CMD_CK) == 0)
		{
			startTime = time_us_64();
		}
	}
	
    pio_sm_set_enabled(PIOInstance::MECHACON, SM::MECHACON, true);
    gpio_acknowledge_irq(Pin::XLAT, GPIO_IRQ_EDGE_FALL);
    gpio_set_irq_enabled(Pin::XLAT, GPIO_IRQ_EDGE_FALL, true);
    
	s_resetPending = 0;
	gpio_set_irq_enabled(Pin::RESET, GPIO_IRQ_LEVEL_LOW, true);
}

