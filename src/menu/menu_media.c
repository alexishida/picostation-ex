#include "menu_media.h"

#include <ctype.h>
#include <string.h>

#include "menu_protocol.h"
#include "ps1/cdrom.h"
#include "ps1/registers.h"
#include "psxproject/cdrom.h"

enum {
    COMMAND_CLEAR = 0x0,
    COMMAND_EXTENDED = 0x8,
    COMMAND_GET_COVER = 0x9,
    EXTENDED_GET_CONFIG = 3,
    COVER_DELAY_FRAMES = 12,
    RESPONSE_ATTEMPTS = 5
};

static uint32_t sectorBuffer[MENU_RAW_SECTOR_BYTES / sizeof(uint32_t)];
static uint32_t coverBuffer[MENU_COVER_RGB15_BYTES / sizeof(uint32_t)];
static uint32_t coverRaw[MENU_COVER_RGB15_PARTS * MENU_RAW_SECTOR_BYTES / sizeof(uint32_t)];
static uint16_t coverIndex;
static unsigned coverDelay;
static unsigned coverWaitFrames;
static bool coverRgb15;

static enum {
    COVER_IDLE,
    COVER_DELAY,
    COVER_READING,
    COVER_MISSING,
    COVER_READY
} coverState;

void sendMenuCommand(uint8_t command, uint16_t argument)
{
    uint8_t parameters[] = {
        CDROM_TEST_DSP_CMD, (uint8_t)(0xf0 | command), (uint8_t)(argument >> 8), (uint8_t)argument
    };
    issueCDROMCommand(CDROM_CMD_TEST, parameters, sizeof(parameters));
}

static uint16_t readLE16(const uint8_t *value)
{
    return value[0] | (uint16_t)value[1] << 8;
}

static bool parseColor(const char *p, const char *end, uint32_t *color)
{
    while (p != end && isspace((unsigned char)*p))
        ++p;
    if (p != end && *p == '#')
        ++p;
    else if (end - p >= 2 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        p += 2;

    uint32_t rgb = 0;
    for (unsigned digit = 0; digit < 6; ++digit)
    {
        if (p == end)
            return false;
        unsigned char c = (unsigned char)*p++;
        if (c >= '0' && c <= '9')
            rgb = (rgb << 4) | (c - '0');
        else if ((c | 32) >= 'a' && (c | 32) <= 'f')
            rgb = (rgb << 4) | ((c | 32) - 'a' + 10);
        else
            return false;
    }

    while (p != end && isspace((unsigned char)*p))
        ++p;
    if (p != end && *p != ';' && *p != '#')
        return false;

    *color = gp0_rgb(rgb >> 16, (rgb >> 8) & 0xff, rgb & 0xff);
    return true;
}

static void parseColors(const char *config, size_t size, MenuColors *colors)
{
    if (size >= 3 && !memcmp(config, "\xef\xbb\xbf", 3))
    {
        config += 3;
        size -= 3;
    }
    const char *end = config + size;
    while (config != end)
    {
        const char *line = config;
        while (config != end && *config != '\n' && *config != '\0')
            ++config;
        const char *lineEnd = config;
        if (config != end && *config == '\n')
            ++config;
        else
            config = end;

        while (line != lineEnd && isspace((unsigned char)*line))
            ++line;
        const char *separator = line;
        while (separator != lineEnd && *separator != '=')
            ++separator;
        if (separator == lineEnd)
            continue;

        const char *keyEnd = separator;
        while (keyEnd != line && isspace((unsigned char)keyEnd[-1]))
            --keyEnd;
        size_t keyLength = keyEnd - line;
        if (keyLength == 10 && !memcmp(line, "background", keyLength))
            parseColor(separator + 1, lineEnd, &colors->background);
        else if (keyLength == 5 && !memcmp(line, "frame", keyLength))
            parseColor(separator + 1, lineEnd, &colors->frame);
        else if (keyLength == 8 && !memcmp(line, "filelist", keyLength))
            parseColor(separator + 1, lineEnd, &colors->fileList);
    }
}

void loadMenuColors(MenuColors *colors)
{
    const uint8_t *data = (const uint8_t *)sectorBuffer + MENU_USER_DATA_OFFSET;
    sendMenuCommand(COMMAND_EXTENDED, EXTENDED_GET_CONFIG << 1);
    for (unsigned attempt = 0; attempt < RESPONSE_ATTEMPTS; ++attempt)
    {
        memset(sectorBuffer, 0, sizeof(sectorBuffer));
        startCDROMRead(MENU_CONFIG_LBA, sectorBuffer, 1, MENU_RAW_SECTOR_BYTES, true, true);
        if (!memcmp(data, "DEAD", 4))
            break;
        if (!memcmp(data, "CFG1", 4))
        {
            uint16_t size = readLE16(data + 4);
            if (size <= MENU_CONFIG_MAX_BYTES)
                parseColors((const char *)data + MENU_CONFIG_DATA_OFFSET, size, colors);
            break;
        }
    }
    sendMenuCommand(COMMAND_CLEAR, 0);
}

static void startCoverRead(void)
{
    coverWaitFrames = 0;
    memset(coverRaw, 0, sizeof(coverRaw));
    startCDROMRead(MENU_COVER_LBA, coverRaw, MENU_COVER_RGB15_PARTS, MENU_RAW_SECTOR_BYTES, true, false);
}

void resetMenuCover(void)
{
    if (coverState == COVER_READING)
    {
        waitForINT1();
        if (waitingForInt5)
            waitForINT2();
        sendMenuCommand(COMMAND_CLEAR, 0);
    }
    coverState = COVER_IDLE;
}

bool updateMenuCover(uint16_t index, TextureInfo *texture)
{
    const bool reading = coverState == COVER_READING;
    // One ReadN fetches every part. INT1 signals each sector, the last
    // sector's Pause completes with INT2, and the final DMA drains after it.
    if (reading && waitingForInt5 &&
        (cdromReadDataNumSectors || (DMA_CHCR(DMA_CDROM) & DMA_CHCR_ENABLE) || waitingForInt2))
    {
        if (++coverWaitFrames < 180)
            return false;
        resetMenuCover();
        coverState = COVER_MISSING;
        return false;
    }

    if (coverState == COVER_IDLE || index != coverIndex)
    {
        if (reading)
            sendMenuCommand(COMMAND_CLEAR, 0);
        coverState = COVER_DELAY;
        coverIndex = index;
        coverDelay = COVER_DELAY_FRAMES;
    }

    if (coverState == COVER_DELAY)
    {
        if (coverDelay-- != 0)
            return false;
        sendMenuCommand(COMMAND_GET_COVER, index);
        coverState = COVER_READING;
        startCoverRead();
        return false;
    }

    if (coverState != COVER_READING)
        return coverState == COVER_READY;

    if (!waitingForInt5)
    {
        coverState = COVER_MISSING;
        sendMenuCommand(COMMAND_CLEAR, 0);
        return false;
    }

    const uint8_t *part0 = (const uint8_t *)coverRaw + MENU_USER_DATA_OFFSET;
    if (!memcmp(part0, "DEAD", 4) || memcmp(part0, "PART", 4) || readLE16(part0 + 4) != 0)
    {
        coverState = COVER_MISSING;
        sendMenuCommand(COMMAND_CLEAR, 0);
        return false;
    }

    coverRgb15 = !memcmp(part0 + MENU_COVER_FORMAT_OFFSET, "RGB5", 4);
    const size_t partSize = coverRgb15 ? MENU_COVER_RGB15_PART_BYTES : MENU_COVER_PART_BYTES;
    const size_t coverSize = coverRgb15 ? MENU_COVER_RGB15_BYTES : MENU_COVER_BYTES;
    const unsigned partCount = coverRgb15 ? MENU_COVER_RGB15_PARTS : MENU_COVER_PARTS;

    bool complete = true;
    for (unsigned part = 0; part < partCount; ++part)
    {
        const uint8_t *data =
            (const uint8_t *)coverRaw + part * MENU_RAW_SECTOR_BYTES + MENU_USER_DATA_OFFSET;
        if (memcmp(data, "PART", 4) || readLE16(data + 4) != part)
        {
            complete = false;
            break;
        }
        const size_t offset = part * partSize;
        const size_t remaining = coverSize - offset;
        const size_t length = remaining < partSize ? remaining : partSize;
        memcpy((uint8_t *)coverBuffer + offset, data + MENU_COVER_DATA_OFFSET, length);
    }

    if (!complete)
    {
        coverState = COVER_MISSING;
        sendMenuCommand(COMMAND_CLEAR, 0);
        return false;
    }

    if (coverRgb15)
        uploadTexture(texture, coverBuffer, 0, 256, MENU_COVER_WIDTH, MENU_COVER_HEIGHT);
    else
        uploadIndexedTexture(texture, (const uint8_t *)coverBuffer + MENU_COVER_PALETTE_BYTES, coverBuffer,
                             768, 0, 768, 128, MENU_COVER_WIDTH, MENU_COVER_HEIGHT, GP0_COLOR_8BPP);
    // Both the artwork and its CLUT reuse the previous selection's VRAM.
    waitForGP0Ready();
    GPU_GP0 = gp0_flushCache();
    coverState = COVER_READY;

    sendMenuCommand(COMMAND_CLEAR, 0);
    return coverState == COVER_READY;
}
