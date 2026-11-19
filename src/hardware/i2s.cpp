#include "menu_protocol.h"
#include "i2s.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <array>

#include "cmd.h"
#include "directory_listing.h"
#include "disc_image.h"
#include "drive_mechanics.h"
#include "ff.h"
#include "global.h"
#include "hardware/pio.h"
#include "logging.h"
#include "main.pio.h"
#include "modchip.h"
#include "pico/stdlib.h"
#include "picostation.h"
#include "pseudo_atomics.h"
#include "subq.h"
#include "values.h"
#include "listingBuilder.h"

#if DEBUG_I2S
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...) while (0)
#endif

pseudoatomic<picostation::FileListingStates> needFileCheckAction;

static uint16_t *__time_critical_func(generateScramblingLUT)()
{
    static uint16_t ScramblingLUT[1176] = {0};
    int shift = 1;
	
	for (int i = 0; i < 6; i++)
	{
		ScramblingLUT[i] = 0;
	}
	
    for (size_t i = 6; i < 1176; i++)
    {
        uint8_t upper = shift & 0xFF;
        for (size_t j = 0; j < 8; j++)
        {
            unsigned bit = ((shift & 1) ^ ((shift & 2) >> 1)) << 15;
            shift = (bit | shift) >> 1;
        }

        uint8_t lower = shift & 0xFF;

        ScramblingLUT[i] = (lower << 8) | upper;

        for (size_t j = 0; j < 8; j++)
        {
            unsigned bit = ((shift & 1) ^ ((shift & 2) >> 1)) << 15;
            shift = (bit | shift) >> 1;
        }
    }

    return ScramblingLUT;
}

int __time_critical_func(picostation::I2S::initDMA)(const volatile void *read_addr, unsigned int transfer_count)
{
    int channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    const unsigned int i2sDREQ = PIOInstance::I2S_DATA == pio0 ? DREQ_PIO0_TX0 : DREQ_PIO1_TX0;
    channel_config_set_dreq(&c, i2sDREQ);
    dma_channel_configure(channel, &c, &PIOInstance::I2S_DATA->txf[SM::I2S_DATA], read_addr, transfer_count, false);

    return channel;
}

void __time_critical_func(picostation::I2S::reinitI2S)()
{
    i2s_state = 0;
    if (dma_channel_is_busy(dmaChannel))
        dma_channel_abort(dmaChannel);
    pio_sm_clear_fifos(PIOInstance::I2S_DATA, SM::I2S_DATA);
    std::fill_n(loadedSector, CACHED_SECS, -2);
    lastSector = -1;
    m_sectorSending = -1;
}

void __time_critical_func(picostation::I2S::resetFromConsole)(bool returnToMenu)
{
    resetRequest = returnToMenu ? ResetRequest::ReturnToMenu : ResetRequest::RestartDisc;
    while (resetRequest.Load() != ResetRequest::None)
        tight_loop_contents();
}

[[noreturn]] void __time_critical_func(picostation::I2S::start)(MechCommand &mechCommand)
{
    picostation::ModChip modChip;
    static uint32_t pioSamples[CACHED_SECS][1176];
    static uint16_t *cdScramblingLUT = generateScramblingLUT();

    uint8_t nextBuffer = 1;
    uint8_t readBuffer = 0;
    uint8_t activeDMABuffer = 0;
    static int currentSector = -1;
    DiscImage::DataLocation dataLocation = DiscImage::DataLocation::RAM;
    static uint32_t loadedImageIndex = 0;
    static uint16_t img_count;
    static DiscImage preparedImage;
    static bool prepared = false;
    static uint32_t preparedIndex;
    static int preparedSectorMax;
    static uint16_t mountStatus[1176] = {};


    needFileCheckAction = picostation::FileListingStates::IDLE;
    
    menu_active = true;
    s_doorPending = false;
    resetRequest = ResetRequest::None;
    dmaChannel = initDMA(pioSamples[0], 1176);
    reinitI2S();

    g_coreReady[1] = true;          // Core 1 is ready
    while (!g_coreReady[0].Load())  // Wait for Core 0 to be ready
    {
        tight_loop_contents();
    }

    modChip.init();

#if DEBUG_I2S
    uint64_t startTime;
    uint64_t endTime;
#endif

    g_discImage.makeDummyCue();
	
    DirectoryListing::init();

    while (true)
    {
        // Sector could change during the loop, so we need to keep track of it
        currentSector = g_driveMechanics.getSector();
        
        modChip.sendLicenseString(currentSector, mechCommand);
		
        const ResetRequest reset = resetRequest.Load();
        const auto action = needFileCheckAction.Load();
        const bool softReturn = action == FileListingStates::RETURN_MENU;
        if (reset != ResetRequest::None || softReturn)
        {
            reinitI2S();
            preparedImage.unload();
            prepared = false;
            if (reset == ResetRequest::ReturnToMenu || softReturn)
            {
                g_discImage.unload();
                g_discImage.makeDummyCue();
                g_discImage.set_skip_bootsector_fix(false);
                g_discImage.set_skip_edc(false);
                dataLocation = DiscImage::DataLocation::RAM;
                menu_active = true;
                if (reset == ResetRequest::ReturnToMenu)
                    DirectoryListing::gotoRoot();
            }
            g_discImage.set_skip_bootsector(false);
            memset(mountStatus, 0, sizeof(mountStatus));
            s_doorPending = false;
            needFileCheckAction = FileListingStates::IDLE;
            if (reset == ResetRequest::None)
                g_driveMechanics.resetDrive();
            resetRequest = ResetRequest::None;
            continue;
        }

        if (menu_active && action != FileListingStates::IDLE)
        {
            switch (action)
            {
                case FileListingStates::GOTO_ROOT:
                case FileListingStates::GOTO_PARENT:
                case FileListingStates::GOTO_DIRECTORY:
                case FileListingStates::GET_NEXT_CONTENTS:
                {
                    if (action == FileListingStates::GOTO_ROOT)
                        DirectoryListing::gotoRoot();
                    else if (action == FileListingStates::GOTO_PARENT)
                        DirectoryListing::gotoParentDirectory();

                    if (action == FileListingStates::GOTO_DIRECTORY &&
                        !DirectoryListing::gotoDirectory(g_fileArg.Load()))
                        DirectoryListing::setListingError(MENU_LIST_OPEN_FAILED);
                    else
                        DirectoryListing::getDirectoryEntries(
                            action == FileListingStates::GET_NEXT_CONTENTS ? g_fileArg.Load() : 0);

                    if (lastSector == 4750)
                        lastSector = -1;
                    needFileCheckAction = FileListingStates::PROCESS_FILES;
                    break;
                }

                case FileListingStates::PREPARE_FILE:
                {
                    preparedImage.unload();
                    prepared = false;
                    const uint32_t index = g_fileArg.Load();
                    char path[c_maxFilePathLength + 1];
                    bool directory;
                    FRESULT result = FR_NO_FILE;
                    if (DirectoryListing::getPath(index, path, &directory) && !directory)
                        result = preparedImage.load(path, false);
                    preparedSectorMax = preparedImage.sectorLimit();
                    prepared = result == FR_OK;
                    preparedIndex = index;
                    if (!prepared)
                        preparedImage.unload();
                    memset(mountStatus, 0, sizeof(mountStatus));
                    memcpy(mountStatus, "MNT1", 4);
                    mountStatus[2] = index;
                    mountStatus[3] = result;
                    if (prepared && !preparedImage.hasData())
                        reinterpret_cast<uint8_t*>(mountStatus)[MENU_MOUNT_FLAGS_OFFSET] = MENU_MOUNT_AUDIO_ONLY;
                    needFileCheckAction = FileListingStates::IDLE;
                    break;
                }

				case picostation::FileListingStates::MOUNT_FILE:
				{
					//printf("Processing MOUNT_FILE\n");
                    char filePath[c_maxFilePathLength + 1];
                    const uint32_t index = g_fileArg.Load();
                    bool isDirectory;
                    needFileCheckAction = FileListingStates::IDLE;
                    if (prepared && preparedIndex == index)
                    {
                        std::swap(g_discImage, preparedImage);
                        preparedImage.unload();
                        prepared = false;
                        c_sectorMax = preparedSectorMax;
                    }
                    else if (!DirectoryListing::getPath(index, filePath, &isDirectory) || isDirectory ||
                        g_discImage.load(filePath) != FR_OK)
                    {
                        break;
                    }

                    loadedImageIndex = index;
                    dataLocation = DiscImage::DataLocation::SDCard;
					img_count = DirectoryListing::getDirectoryEntriesCount();
					menu_active = false;
					reinitI2S();
					g_driveMechanics.resetDrive();
					g_discImage.set_skip_bootsector(false);
					g_discImage.set_skip_bootsector_fix(false);
					mechCommand.resetBootSectorPattern();

					continue;
					break;
				}
				
                case FileListingStates::GET_COVER:
                    DirectoryListing::openCover(g_fileArg.Load());
                    needFileCheckAction = FileListingStates::PROCESS_FILES;
                    break;

                case FileListingStates::GET_CFG:
                    DirectoryListing::openCfg();
                    needFileCheckAction = FileListingStates::PROCESS_FILES;
                    break;

				default:
					break;
			}
		}
		else if (s_doorPending.Load() && !menu_active)
		{
			s_doorPending = false;
            for (uint32_t step = 1; step <= img_count; ++step)
            {
                const uint32_t index = (loadedImageIndex + step) % img_count;
                char filePath[c_maxFilePathLength + 1];
                bool isDirectory;
                if (DirectoryListing::getPath(index, filePath, &isDirectory) && !isDirectory &&
                    g_discImage.load(filePath) == FR_OK)
                {
                    loadedImageIndex = index;
                    reinitI2S();
                    g_driveMechanics.resetDrive();
                    currentSector = g_driveMechanics.getSector();
                    break;
                }
            }
		}
		
        // Data sent via DMA, load the next sector
        if (currentSector != lastSector && currentSector >= 4503 && currentSector < c_sectorMax)
        {
			if (!menu_active)
			{
				for (int i = 0; i < CACHED_SECS; i++)
				{
					if (loadedSector[i] == currentSector)
					{
						// already in cache
						nextBuffer = i;
						lastSector = currentSector;
#if DEBUG_I2S0
						DEBUG_PRINT("sector %d in cache\n", currentSector);
#endif
						goto continue_transfer;
					}
				}
			}
			
            const bool dmaBusy = dma_channel_is_busy(dmaChannel);
            while (readBuffer == nextBuffer || (dmaBusy && readBuffer == activeDMABuffer))
			{
                readBuffer = (readBuffer + 1) & (CACHED_SECS - 1);
			}
			
            bool cacheable = true;
            if (menu_active && currentSector == c_leadIn + c_preGap + MENU_MOUNT_STATUS_LBA)
            {
                g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], mountStatus, cdScramblingLUT);
                cacheable = false;
            }
            else if (menu_active && needFileCheckAction.Load() == FileListingStates::PROCESS_FILES)
			{
				if (currentSector == 4750)
				{
					g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], picostation::DirectoryListing::getFileListingData(), cdScramblingLUT);
					needFileCheckAction = picostation::FileListingStates::IDLE;
				}
				else if (currentSector >= c_leadIn + c_preGap + MENU_COVER_LBA &&
                         currentSector < c_leadIn + c_preGap + MENU_COVER_LBA + MENU_COVER_RGB15_PARTS)
				{
					g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], picostation::DirectoryListing::readCover(currentSector - c_leadIn - c_preGap - MENU_COVER_LBA), cdScramblingLUT);
				}
				else if (currentSector == c_leadIn + c_preGap + MENU_CONFIG_LBA)
				{
					g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], picostation::DirectoryListing::readCfg(), cdScramblingLUT);
				}
				else
				{
					goto not_menu_request;
				}
			}
			else
			{
not_menu_request:
#if DEBUG_I2S
				startTime = time_us_64();
#endif
				// Load the next sector
				cacheable = g_discImage.readSector(pioSamples[readBuffer], currentSector - c_leadIn, dataLocation,
                                                  cdScramblingLUT);
#if DEBUG_I2S
				endTime = time_us_64()-startTime;
				
				if (endTime > 5000)
				{
					DEBUG_PRINT("read time: %lluus (%d)\n", endTime, currentSector);
				}
#endif
			}
			
			loadedSector[readBuffer] = cacheable ? currentSector : -2;
			nextBuffer = readBuffer;
			lastSector = currentSector;
		}

continue_transfer:

        // Start the next transfer if the DMA channel is not busy
        if (!dma_channel_is_busy(dmaChannel) && i2s_state.Load())
        {
			if (currentSector >= 4503 && currentSector < c_sectorMax)
			{
                // Wait for the rising edge, allowing the main loop to handle a stopped clock.
                const uint32_t syncStart = time_us_32();
                while (gpio_get(Pin::LRCK) && time_us_32() - syncStart < 100)
                    tight_loop_contents();
                while (!gpio_get(Pin::LRCK) && time_us_32() - syncStart < 100)
                    tight_loop_contents();
                if (time_us_32() - syncStart >= 100 || !i2s_state.Load() ||
                    resetRequest.Load() != ResetRequest::None ||
                    g_driveMechanics.getSector() != currentSector)
                    continue;

                activeDMABuffer = nextBuffer;
                dma_channel_set_read_addr(dmaChannel, pioSamples[activeDMABuffer], true);
                m_lastSectorTime = time_us_64();
                m_sectorSending = lastSector;
			}
			else if (!g_subqDelay.Load())
			{
				m_lastSectorTime = time_us_64();
				m_sectorSending = currentSector;
			}
        }
    }
    __builtin_unreachable();
}
