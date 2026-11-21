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

#ifdef PICOSTATION_SD_BOOT_LOG
namespace {
struct BootRun {
    uint32_t startUs, endUs;
    int firstLba, lastLba;
    unsigned count;
    uint8_t flags;
};

// Preserve the beginning and the most recent runs if the console retries for a long time.
constexpr unsigned firstRuns = 24;
constexpr unsigned recentRuns = 104;
BootRun bootRuns[firstRuns + recentRuns];
unsigned runCount;
unsigned lbaTransfers[80];

BootRun& bootRun(unsigned index)
{
    return bootRuns[index < firstRuns ? index : firstRuns + (index - firstRuns) % recentRuns];
}

void traceTransfer(uint32_t us, int lba, uint8_t flags)
{
    if (lba >= 0 && static_cast<unsigned>(lba) < std::size(lbaTransfers))
        ++lbaTransfers[lba];
    if (runCount)
    {
        BootRun& run = bootRun(runCount - 1);
        const bool repeat = lba == run.lastLba && run.firstLba == run.lastLba;
        const bool next = lba == run.lastLba + 1 &&
                          run.lastLba - run.firstLba == static_cast<int>(run.count - 1);
        if (flags == run.flags && (repeat || next) && us - run.endUs < 100000)
        {
            run.lastLba = lba;
            run.endUs = us;
            ++run.count;
            return;
        }
    }
    bootRun(runCount++) = {us, us, lba, lba, 1, flags};
}

void writeBootTrace()
{
    // Batch lines so FatFs does not reopen and sync the file for every run.
    static char batch[1024];
    size_t used = 0;
    auto line = [&](const char* format, auto... values) {
        char entry[112];
        const int length = snprintf(entry, sizeof(entry), format, values...);
        if (length <= 0 || static_cast<size_t>(length) >= sizeof(entry))
            return;
        if (used + length >= sizeof(batch))
        {
            batch[used] = 0;
            picostation::DirectoryListing::appendBootLog(batch);
            used = 0;
        }
        memcpy(batch + used, entry, length);
        used += length;
    };

    line("DMA grupos=%u (flags: bit0=GFS bit1=SOCT bit2=SubQ pendente)\r\n", runCount);
    line("%s", "grupo inicio_us fim_us primeiro_lba ultimo_lba transferencias flags\r\n");
    for (unsigned i = 0; i < runCount; ++i)
    {
        if (i == firstRuns && runCount > std::size(bootRuns))
        {
            line("grupos intermediarios omitidos=%u\r\n", runCount - unsigned(std::size(bootRuns)));
            i = runCount - recentRuns;
        }
        const BootRun& r = bootRun(i);
        line("%u %u %u %d %d %u %u\r\n", i, unsigned(r.startUs), unsigned(r.endUs),
             r.firstLba, r.lastLba, r.count, unsigned(r.flags));
    }
    line("%s", "Total DMA por LBA 0-79 (ausente=zero)\r\n");
    for (unsigned i = 0; i < std::size(lbaTransfers); ++i)
        if (lbaTransfers[i])
            line("lba=%u dma=%u\r\n", i, lbaTransfers[i]);
    line("%s", "FIM TRACE DMA\r\n");
    batch[used] = 0;
    picostation::DirectoryListing::appendBootLog(batch);
}
}  // namespace
#endif

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

void __time_critical_func(picostation::I2S::i2s_set_state)(uint8_t state)
{
    critical_section_enter_blocking(&transferLock);
    transfer.enabled = state != 0;
    if (!state)
        ++transfer.generation;
    critical_section_exit(&transferLock);
}

void __time_critical_func(picostation::I2S::publishDriveSector)(int sector)
{
    critical_section_enter_blocking(&transferLock);
    transfer.sector = sector;
    critical_section_exit(&transferLock);
}

void picostation::I2S::setReadOption(DiscImage::ReadOption option, bool enabled)
{
    critical_section_enter_blocking(&transferLock);
    if (enabled)
        transfer.options |= option;
    else
        transfer.options &= ~option;
    critical_section_exit(&transferLock);
}

picostation::I2S::TransferState __time_critical_func(picostation::I2S::transferSnapshot)()
{
    critical_section_enter_blocking(&transferLock);
    const auto state = transfer;
    critical_section_exit(&transferLock);
    return state;
}

picostation::I2S::TransferResult __time_critical_func(picostation::I2S::startSectorDMA)(const TransferState& expected)
{
    critical_section_enter_blocking(&transferLock);
    const bool ready = transfer.enabled && transfer.sector == expected.sector &&
                       transfer.generation == expected.generation && transfer.options == expected.options &&
                       resetRequest.Load() == ResetRequest::None;
    TransferResult result = TransferResult::Stale;
    if (ready)
    {
        // Keep seek/reset serialized, with only the trigger after the rising edge.
        const uint32_t syncStart = time_us_32();
        const auto waitForLRCK = [syncStart](bool level) {
            while (gpio_get(Pin::LRCK) != level)
                if (time_us_32() - syncStart >= 100)
                    return false;
            return true;
        };
        result = TransferResult::ClockTimeout;
        if (waitForLRCK(false) && waitForLRCK(true))
        {
            dma_channel_start(dmaChannel);
            result = TransferResult::Started;
        }
    }
    critical_section_exit(&transferLock);
    return result;
}

void picostation::I2S::enableDoor(bool enabled)
{
    critical_section_enter_blocking(&transferLock);
    doorEnabled = enabled;
    doorPending = 0;
    critical_section_exit(&transferLock);
}

void picostation::I2S::notifyDoorOpened()
{
    critical_section_enter_blocking(&transferLock);
    if (doorEnabled)
        doorPending |= DoorOpened;
    critical_section_exit(&transferLock);
}

void picostation::I2S::notifyDoor()
{
    critical_section_enter_blocking(&transferLock);
    if (doorEnabled)
        doorPending |= DoorCycle;
    critical_section_exit(&transferLock);
}

uint8_t picostation::I2S::takeDoorEvent()
{
    critical_section_enter_blocking(&transferLock);
    const uint8_t pending = doorPending;
    doorPending = 0;
    critical_section_exit(&transferLock);
    return pending;
}

void __time_critical_func(picostation::I2S::reinitI2S)()
{
    i2s_set_state(0);
    if (dma_channel_is_busy(dmaChannel))
        dma_channel_abort(dmaChannel);
    pio_sm_clear_fifos(PIOInstance::I2S_DATA, SM::I2S_DATA);
    std::fill_n(loadedSector, CACHED_SECS, -2);
    lastSector = -1;
    m_sectorSending = -1;
}

void __time_critical_func(picostation::I2S::resetFromConsole)(bool returnToMenu)
{
    critical_section_enter_blocking(&transferLock);
    resetRequest = returnToMenu ? ResetRequest::ReturnToMenu : ResetRequest::RestartDisc;
    critical_section_exit(&transferLock);
    while (resetRequest.Load() != ResetRequest::None)
        serviceDiscPause(true);
}

[[noreturn]] void __time_critical_func(picostation::I2S::start)(MechCommand &mechCommand)
{
    picostation::ModChip modChip;
    static uint32_t pioSamples[CACHED_SECS][1176];
    static uint16_t *cdScramblingLUT = generateScramblingLUT();

    uint8_t nextBuffer = 1;
    uint8_t readBuffer = 0;
    uint8_t activeDMABuffer = 0;
    bool listingReady = false;
    uint8_t cachedOptions = 0;
    DiscImage::DataLocation dataLocation = DiscImage::DataLocation::RAM;
    static uint32_t loadedImageIndex = 0;
    static uint16_t img_count;
    static DiscImage preparedImage;
    static DiscImage previousImage;
    static bool prepared = false;
    static uint32_t preparedIndex;
    static char preparedPath[c_maxFilePathLength + 1];
    static int preparedSectorMax;
    static uint16_t mountStatus[1176] = {};


    needFileCheckAction = picostation::FileListingStates::IDLE;
    
    menu_active = true;
    enableDoor(false);
    resetRequest = ResetRequest::None;
    dmaChannel = initDMA(pioSamples[0], 1176);
    reinitI2S();
    g_discImage.makeDummyCue();

    const auto publishImage = [&](DiscImage& image, int sectorLimit, bool core0Resetting) {
        if (!core0Resetting)
            requestDiscPause();
        g_discImage.replacePreparedImage(image, previousImage);
        c_sectorMax = sectorLimit;
        if (!core0Resetting)
            releaseDiscPause();
        previousImage.unload();
    };

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

    DirectoryListing::init();
#ifdef PICOSTATION_SD_BOOT_LOG
    uint32_t bootStart = time_us_32();
    bool bootReported = false;
    unsigned transfers = 0;
    unsigned syncTimeouts = 0;
    unsigned resets = 0;
    unsigned cancelledTransfers = 0;
    unsigned configRequests = 0;
    unsigned configBuilt = 0;
    unsigned configValid = 0;
    unsigned configLbaTransfers = 0;
    unsigned listingRequests = 0;
    unsigned listingSectorsBuilt = 0;
    unsigned listingLbaTransfers = 0;
    unsigned listingErrors = 0;
    int lastBootLba = 0;
    bool sentPvd = false;
#endif
    bool menuConfigSeen = false;
    const auto restartMenuLog = [&]() {
        DirectoryListing::beginMenuLog();
        menuConfigSeen = false;
#ifdef PICOSTATION_SD_BOOT_LOG
        bootStart = time_us_32();
        bootReported = sentPvd = false;
        transfers = syncTimeouts = resets = cancelledTransfers = 0;
        configRequests = configBuilt = configValid = configLbaTransfers = 0;
        listingRequests = listingErrors = listingSectorsBuilt = listingLbaTransfers = 0;
        lastBootLba = runCount = 0;
        std::fill_n(lbaTransfers, std::size(lbaTransfers), 0);
#endif
    };

    while (true)
    {
        DirectoryListing::flushErrors();
        // Sector could change during the loop, so we need to keep track of it
        const auto state = transferSnapshot();
        const int currentSector = state.sector;
        
        modChip.sendLicenseString(currentSector, mechCommand);
		
        const ResetRequest reset = resetRequest.Load();
        const auto action = needFileCheckAction.Load();
        const bool softReturn = action == FileListingStates::RETURN_MENU;
#ifdef PICOSTATION_SD_BOOT_LOG
        if (reset != ResetRequest::None)
            ++resets;
        if (!bootReported && time_us_32() - bootStart >= 45000000)
        {
            char line[160];
            snprintf(line, sizeof(line),
                     "boot_ms=%u dma=%u ultimo_lba=%d pvd_lba16=%u lrck_timeout=%u comando=%u resets=%u\r\n",
                     (unsigned)((time_us_32() - bootStart) / 1000), transfers, lastBootLba,
                     (unsigned)sentPvd, syncTimeouts, (unsigned)action, resets);
            DirectoryListing::appendBootLog(line);
            snprintf(line, sizeof(line),
                     "setor_drive=%d i2s=%u dma_ocupado=%u cancelados_posicao_ou_reset=%u\r\n",
                     currentSector, unsigned(state.enabled), unsigned(dma_channel_is_busy(dmaChannel)),
                     cancelledTransfers);
            DirectoryListing::appendBootLog(line);
            snprintf(line, sizeof(line), "lista_pedidos=%u lista_erros=%u lista_setores_montados=%u dma_lba100=%u\r\n",
                     listingRequests, listingErrors, listingSectorsBuilt, listingLbaTransfers);
            DirectoryListing::appendBootLog(line);
            snprintf(line, sizeof(line), "cfg_pedidos=%u cfg_setores_montados=%u cfg_validos=%u dma_lba150=%u\r\n",
                     configRequests, configBuilt, configValid, configLbaTransfers);
            DirectoryListing::appendBootLog(line);
            writeBootTrace();
            bootReported = true;
        }
#endif
        if (reset != ResetRequest::None || softReturn)
        {
            if (reset != ResetRequest::None && (menu_active || reset == ResetRequest::ReturnToMenu))
                restartMenuLog();
            reinitI2S();
            listingReady = false;
            preparedImage.unload();
            prepared = false;
            if (reset == ResetRequest::ReturnToMenu || softReturn)
            {
                preparedImage.makeDummyCue(false);
                publishImage(preparedImage, 333000, reset != ResetRequest::None);
                setReadOption(DiscImage::SkipBootSectorFix, false);
                setReadOption(DiscImage::SkipEdc, false);
                dataLocation = DiscImage::DataLocation::RAM;
                menu_active = true;
                if (reset == ResetRequest::ReturnToMenu)
                    DirectoryListing::gotoRoot();
            }
            setReadOption(DiscImage::SkipBootSector, false);
            memset(mountStatus, 0, sizeof(mountStatus));
            enableDoor(!menu_active);
            needFileCheckAction = FileListingStates::IDLE;
            if (reset != ResetRequest::None)
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

#ifdef PICOSTATION_SD_BOOT_LOG
                    ++listingRequests;
#endif
                    if (action == FileListingStates::GOTO_DIRECTORY &&
                        !DirectoryListing::gotoDirectory(g_fileArg.Load()))
                    {
                        DirectoryListing::setListingError(MENU_LIST_OPEN_FAILED);
#ifdef PICOSTATION_SD_BOOT_LOG
                        ++listingErrors;
#endif
                    }
                    else
                    {
                        [[maybe_unused]] const bool listed = DirectoryListing::getDirectoryEntries(
                            action == FileListingStates::GET_NEXT_CONTENTS ? g_fileArg.Load() : 0);
#ifdef PICOSTATION_SD_BOOT_LOG
                        if (!listed)
                            ++listingErrors;
#endif
                    }

                    listingReady = true;
                    if (lastSector == 4750)
                        lastSector = -1;
                    needFileCheckAction = FileListingStates::IDLE;
                    break;
                }

                case FileListingStates::PREPARE_FILE:
                {
                    preparedImage.unload();
                    prepared = false;
                    const uint32_t index = g_fileArg.Load();
                    bool directory;
                    FRESULT result = FR_NO_FILE;
                    if (DirectoryListing::getPath(index, preparedPath, &directory) && !directory)
                        result = preparedImage.load(preparedPath, false);
                    prepared = result == FR_OK;
                    if (prepared)
                        preparedSectorMax = preparedImage.sectorLimit();
                    preparedIndex = index;
                    if (!prepared)
                    {
                        DirectoryListing::recordError(DirectoryListing::Error::Prepare, result);
                        preparedImage.unload();
                    }
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
                    const uint32_t index = g_fileArg.Load();
                    bool isDirectory;
                    needFileCheckAction = FileListingStates::IDLE;
                    if (!(prepared && preparedIndex == index))
                    {
                        preparedImage.unload();
                        prepared = false;
                        if (!DirectoryListing::getPath(index, preparedPath, &isDirectory) || isDirectory ||
                            preparedImage.load(preparedPath, false) != FR_OK)
                        {
                            DirectoryListing::recordError(DirectoryListing::Error::MountImage, index);
                            break;
                        }
                        preparedSectorMax = preparedImage.sectorLimit();
                    }
                    reinitI2S();
                    publishImage(preparedImage, preparedSectorMax, false);
                    prepared = false;

                    loadedImageIndex = index;
                    dataLocation = DiscImage::DataLocation::SDCard;
					img_count = DirectoryListing::getDirectoryEntriesCount();
					menu_active = false;
                    enableDoor(true);
                    DirectoryListing::logDiscEvent("jogo_montado", preparedPath);
					setReadOption(DiscImage::SkipBootSector, false);
					setReadOption(DiscImage::SkipBootSectorFix, false);
					mechCommand.resetBootSectorPattern();

					continue;
					break;
				}
				
                case FileListingStates::GET_COVER:
                    DirectoryListing::openCover(g_fileArg.Load());
                    needFileCheckAction = FileListingStates::PROCESS_FILES;
                    break;

                case FileListingStates::GET_CFG:
                    if (menuConfigSeen)
                        restartMenuLog();
                    menuConfigSeen = true;
#ifdef PICOSTATION_SD_BOOT_LOG
                    ++configRequests;
#endif
                    DirectoryListing::openCfg();
                    needFileCheckAction = FileListingStates::PROCESS_FILES;
                    break;

				default:
					break;
			}
		}
		else if (const auto events = menu_active ? 0 : takeDoorEvent())
		{
            if (events & DoorOpened)
                DirectoryListing::logDiscEvent("tampa_sinal_alto");
            if (!(events & DoorCycle))
                continue;
            DirectoryListing::logDiscEvent("tampa_ciclo_confirmado");
            bool mounted = false;
            for (uint32_t step = 1; step <= img_count; ++step)
            {
                const uint32_t index = (loadedImageIndex + step) % img_count;
                char filePath[c_maxFilePathLength + 1];
                bool isDirectory;
                preparedImage.unload();
                prepared = false;
                if (!DirectoryListing::getPath(index, filePath, &isDirectory) || isDirectory)
                    continue;
                if (preparedImage.load(filePath, false) == FR_OK)
                {
                    const bool sameDisc = loadedImageIndex == index;
                    loadedImageIndex = index;
                    reinitI2S();
                    publishImage(preparedImage, preparedImage.sectorLimit(), false);
                    mounted = true;
                    DirectoryListing::logDiscEvent(sameDisc ? "mesmo_disco_remontado" : "disco_trocado", filePath);
                    break;
                }
                DirectoryListing::recordError(DirectoryListing::Error::MountImage, index);
            }
            if (!mounted)
                DirectoryListing::logDiscEvent("troca_sem_imagem_valida");
            continue;
		}
		
        if (cachedOptions != state.options)
        {
            std::fill_n(loadedSector, CACHED_SECS, -2);
            lastSector = -1;
            cachedOptions = state.options;
        }

        // Data sent via DMA, load the next sector
        if (currentSector != lastSector && currentSector >= 4503 && currentSector < c_sectorMax.Load())
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
            else if (menu_active && listingReady && currentSector == 4750)
            {
                // The console can seek back to the same page after the first DMA attempt.
                g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer],
                                        DirectoryListing::getFileListingData(), cdScramblingLUT);
                cacheable = false;
#ifdef PICOSTATION_SD_BOOT_LOG
                ++listingSectorsBuilt;
#endif
            }
            else if (menu_active && needFileCheckAction.Load() == FileListingStates::PROCESS_FILES)
            {
                if (currentSector >= c_leadIn + c_preGap + MENU_COVER_LBA &&
                         currentSector < c_leadIn + c_preGap + MENU_COVER_LBA + MENU_COVER_RGB15_PARTS)
				{
					g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], picostation::DirectoryListing::readCover(currentSector - c_leadIn - c_preGap - MENU_COVER_LBA), cdScramblingLUT);
				}
				else if (currentSector == c_leadIn + c_preGap + MENU_CONFIG_LBA)
				{
					auto* config = DirectoryListing::readCfg();
#ifdef PICOSTATION_SD_BOOT_LOG
                    ++configBuilt;
                    if (memcmp(config, "CFG1", 4) == 0)
                        ++configValid;
#endif
                    g_discImage.buildSector(currentSector - c_leadIn, pioSamples[readBuffer], config, cdScramblingLUT);
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
                                                  cdScramblingLUT, state.options);
                if (!cacheable && dataLocation == DiscImage::DataLocation::SDCard)
                    DirectoryListing::recordError(DirectoryListing::Error::SectorRead, currentSector - c_leadIn - c_preGap);
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
        if (!dma_channel_is_busy(dmaChannel) && state.enabled)
        {
			if (currentSector >= 4503 && currentSector < c_sectorMax.Load())
			{
				// The rc5 reference published the sector identity and its timing
				// before touching the DMA. The core-0 subchannel scheduler gates
				// on getSectorSending() == currentSector, so a freshly
				// published pair keeps the Q subchannel advancing even while
				// the LRCK edge wait or a command transition stalls the actual
				// transfer. Publishing only after a successful trigger, as the
				// refactor did, freezes the pair on the previously delivered
				// sector during those windows and stops the Q subchannel from
				// completing the tail of long reads.
				m_sectorSending = lastSector;
				m_lastSectorTime = time_us_64();

                dma_channel_set_read_addr(dmaChannel, pioSamples[nextBuffer], false);

                const auto result = startSectorDMA(state);
                if (result == TransferResult::ClockTimeout)
                {
                    DirectoryListing::recordError(DirectoryListing::Error::LRCK, currentSector);
#ifdef PICOSTATION_SD_BOOT_LOG
                    if (!bootReported)
                        ++syncTimeouts;
#endif
                    continue;
                }
                if (result == TransferResult::Stale)
                {
#ifdef PICOSTATION_SD_BOOT_LOG
                    if (!bootReported)
                        ++cancelledTransfers;
#endif
                    continue;
                }
                activeDMABuffer = nextBuffer;
#ifdef PICOSTATION_SD_BOOT_LOG
                if (!bootReported)
                {
                    ++transfers;
                    lastBootLba = lastSector - c_leadIn - c_preGap;
                    sentPvd |= lastBootLba == 16;
                    if (menu_active && lastBootLba == MENU_CONFIG_LBA)
                        ++configLbaTransfers;
                    if (menu_active && lastBootLba == 100)
                        ++listingLbaTransfers;
                    const uint8_t flags = unsigned(mechCommand.getGfsShared()) |
                                          (unsigned(mechCommand.getSoct()) << 1) |
                                          (unsigned(g_subqDelay.Load()) << 2);
                    traceTransfer(time_us_32() - bootStart, lastBootLba, flags);
                }
#endif
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
