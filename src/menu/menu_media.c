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
    COVER_DELAY_FRAMES = 40,
    RESPONSE_ATTEMPTS = 5
};

static uint32_t sectorBuffer[MENU_RAW_SECTOR_BYTES / sizeof(uint32_t)];
static uint32_t coverBuffer[MENU_COVER_RGB15_BYTES / sizeof(uint32_t)];
static uint16_t coverIndex;
static unsigned coverPart;
static unsigned coverDelay;
static unsigned coverAttempts;
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

static void readCoverPart(void)
{
    coverWaitFrames = 0;
    memset(sectorBuffer, 0, sizeof(sectorBuffer));
    startCDROMRead(MENU_COVER_LBA + coverPart, sectorBuffer, 1, MENU_RAW_SECTOR_BYTES, true, false);
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
    // INT1 starts CD DMA. Its Pause command completes later with INT2.
    if (reading && waitingForInt5 && (waitingForInt1 || waitingForInt2))
    {
        if (++coverWaitFrames < 180)
            return false;
        resetMenuCover();
        coverState = COVER_MISSING;
        return false;
    }
    if (reading && (DMA_CHCR(DMA_CDROM) & DMA_CHCR_ENABLE))
        return false;

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
        coverPart = 0;
        coverAttempts = 0;
        sendMenuCommand(COMMAND_GET_COVER, index);
        coverState = COVER_READING;
        readCoverPart();
        return false;
    }

    if (coverState != COVER_READING)
        return coverState == COVER_READY;

    const uint8_t *data = (const uint8_t *)sectorBuffer + MENU_USER_DATA_OFFSET;
    if (waitingForInt5 && !memcmp(data, "PART", 4) && readLE16(data + 4) == coverPart)
    {
        if (coverPart == 0)
            coverRgb15 = !memcmp(data + MENU_COVER_FORMAT_OFFSET, "RGB5", 4);
        const size_t partSize = coverRgb15 ? MENU_COVER_RGB15_PART_BYTES : MENU_COVER_PART_BYTES;
        const size_t coverSize = coverRgb15 ? MENU_COVER_RGB15_BYTES : MENU_COVER_BYTES;
        const unsigned partCount = coverRgb15 ? MENU_COVER_RGB15_PARTS : MENU_COVER_PARTS;
        size_t offset = coverPart * partSize;
        size_t remaining = coverSize - offset;
        size_t length = remaining < partSize ? remaining : partSize;
        memcpy((uint8_t *)coverBuffer + offset, data + MENU_COVER_DATA_OFFSET, length);
        coverAttempts = 0;
        if (++coverPart < partCount)
        {
            readCoverPart();
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
    }
    else if (!memcmp(data, "DEAD", 4) || ++coverAttempts == RESPONSE_ATTEMPTS)
    {
        coverState = COVER_MISSING;
    }
    else
    {
        readCoverPart();
        return false;
    }

    sendMenuCommand(COMMAND_CLEAR, 0);
    return coverState == COVER_READY;
}
