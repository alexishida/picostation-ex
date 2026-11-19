#include "menu_media.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "psxproject/cdrom.h"
#include "psxproject/filesystem.h"

// Dates from the reference menu's lookup for discs that boot a generic PSX.EXE.
static const struct {
    char creationDate[17];
    char serial[7];
    bool scps;
} legacyGameIds[] = {
    { "1994111009000000", "000.01", false },
    { "1994110702000000", "000.02", false },
    { "1994102615231700", "000.03", false },
    { "1994110218594700", "000.04", false },
    { "1995030218052000", "000.04", false },
    { "1994110722360400", "000.05", false },
    { "1994120610494900", "000.05", false },
    { "1994110407000000", "000.06", false },
    { "1994111419300000", "000.07", false },
    { "1994121808190700", "000.08", false },
    { "1994121917000000", "000.09", false },
    { "1995052918000000", "000.10", false },
    { "1994110220020600", "000.11", false },
    { "1994121518000000", "000.13", false },
    { "1994103000000000", "000.14", false },
    { "1994101813262400", "000.15", false },
    { "1994112617300000", "000.16", false },
    { "1994121517300000", "000.16", false },
    { "1994111013000000", "000.17", false },
    { "1994111522183200", "000.18", false },
    { "1994112918000000", "000.19", false },
    { "1994111721302100", "000.20", false },
    { "1994100617242100", "000.21", false },
    { "1995030215000000", "000.22", false },
    { "1994122718351900", "000.23", false },
    { "1994092920284600", "000.24", false },
    { "1994113012000000", "000.25", false },
    { "1995012512000000", "000.25", false },
    { "1995041921063500", "000.26", false },
    { "1994121500000000", "000.27", false },
    { "1994121017582300", "000.28", false },
    { "1995022623000000", "000.29", false },
    { "1995050116000000", "000.30", false },
    { "1995060613000000", "000.30", false },
    { "1995021802000000", "000.31", false },
    { "1995021615022900", "000.32", false },
    { "1995100209000000", "000.33", false },
    { "1995080809000000", "000.33", false },
    { "1995071821394900", "000.34", false },
    { "1995042506300000", "000.35", false },
    { "1995011411551700", "000.37", false },
    { "1995041311392800", "000.38", false },
    { "1995031205000000", "000.40", false },
    { "1995061612000000", "000.40", false },
    { "1995040509000000", "000.41", false },
    { "1995052612000000", "000.43", false },
    { "1995042500000000", "000.44", false },
    { "1995033100003000", "000.47", false },
    { "1995041400000000", "000.48", false },
    { "1995050413421800", "000.50", false },
    { "1995040509595900", "000.51", false },
    { "1995030103150000", "000.52", false },
    { "1995100409235300", "000.53", false },
    { "1995060504013600", "000.55", false },
    { "1995060319142200", "000.55", false },
    { "1995060402110800", "000.55", false },
    { "1995081612000000", "000.59", false },
    { "1995051201000000", "000.60", false },
    { "1995051700000000", "000.61", false },
    { "1995051002471900", "000.63", false },
    { "1995083112000000", "000.64", false },
    { "1995111700000000", "000.64", false },
    { "1996033100000000", "000.65", false },
    { "1995051816000000", "000.66", false },
    { "1995061418000000", "000.67", false },
    { "1995061911303400", "000.68", false },
    { "1995072800300000", "000.68", false },
    { "1995061806364400", "000.69", false },
    { "1995062922000000", "000.70", false },
    { "1995040719355400", "000.71", false },
    { "1995051015300000", "000.77", false },
    { "1995070302000000", "000.78", false },
    { "1995070523450000", "000.83", false },
    { "1995072522004900", "000.85", false },
    { "1995070613170000", "000.88", false },
    { "1995082517551900", "000.89", false },
    { "1995082109402500", "000.90", false },
    { "1995053117000000", "000.91", false },
    { "1995081100000000", "000.92", false },
    { "1995071011035200", "000.93", false },
    { "1995090510000000", "000.94", false },
    { "1995083123000000", "000.94", false },
    { "1995100601300000", "000.99", false },
    { "1995081001450000", "001.01", false },
    { "1995080316000000", "001.03", false },
    { "1995081020000000", "001.04", false },
    { "1995090722000000", "001.08", false },
    { "1995090516062841", "001.13", false },
    { "1995082016003000", "001.28", false },
    { "1995102101350000", "001.33", false },
    { "1995102102521200", "001.33", false },
    { "1995102105003200", "001.33", false },
    { "1995100910002200", "001.37", false },
    { "1995101801325900", "001.42", false },
    { "1995113010450000", "001.46", false },
    { "1995092205430500", "001.52", false },
    { "1995121620000000", "001.73", false },
    { "1995030717020700", "100.02", true },
    { "1995032400000000", "100.07", true },
    { "1995052420065100", "100.08", true },
};

static const char *findBootPath(const char *config, size_t size, char path[2049])
{
    const char *end = config + size;
    for (const char *line = config; line < end && *line; )
    {
        const char *next = line;
        while (next < end && *next && *next != '\n')
            ++next;

        while (line != next && isspace((unsigned char)*line))
            ++line;
        if (next - line >= 4 && !memcmp(line, "BOOT", 4))
        {
            const char *value = line + 4;
            while (value != next && isspace((unsigned char)*value))
                ++value;
            if (value != next && *value == '=')
                ++value;
            else
                value = next;
            while (value != next && isspace((unsigned char)*value))
                ++value;
            const char *valueEnd = value;
            while (valueEnd != next && !isspace((unsigned char)*valueEnd))
                ++valueEnd;
            if (valueEnd != value)
            {
                memcpy(path, value, valueEnd - value);
                path[valueEnd - value] = '\0';
                return path;
            }
        }
        if (next == end || !*next)
            break;
        line = next + 1;
    }
    return "cdrom:PS.EXE;1";
}

bool getMenuGameId(void *sectorBuffer, char gameId[20], char executable[40])
{
    if (initFilesystem())
        return false;

    char path[2049];
    const char *bootPath = "cdrom:PS.EXE;1";
    if (file_load("SYSTEM.CNF;1", sectorBuffer) == 0 && waitingForInt5)
        bootPath = findBootPath(sectorBuffer, 2048, path);
    snprintf(executable, 40, "%s", bootPath);

    const char *filename = bootPath;
    for (const char *p = bootPath; *p; ++p)
        if (*p == ':' || *p == '/' || *p == '\\')
            filename = p + 1;

    const char *version = strchr(filename, ';');
    const size_t nameLength = version ? (size_t)(version - filename) : strlen(filename);
    const char genericName[] = "PSX.EXE";
    bool generic = nameLength == sizeof(genericName) - 1;
    for (size_t i = 0; generic && i < nameLength; ++i)
        generic = toupper((unsigned char)filename[i]) == genericName[i];

    if (generic)
    {
        const char *serial = "0";
        const char *prefix = "SLPS_";
        startCDROMRead(16, sectorBuffer, 1, 2048, true, true);
        if (waitingForInt5)
        {
            const char *date = (const char *)sectorBuffer + 813;
            for (unsigned i = 0; i < sizeof(legacyGameIds) / sizeof(legacyGameIds[0]); ++i)
            {
                if (!memcmp(date, legacyGameIds[i].creationDate, 16))
                {
                    serial = legacyGameIds[i].serial;
                    if (legacyGameIds[i].scps)
                        prefix = "SCPS_";
                    break;
                }
            }
        }
        snprintf(gameId, 20, "cdrom:%s%s;1", prefix, serial);
        return true;
    }

    const size_t prefixLength = sizeof("cdrom:") - 1;
    const size_t filenameLength = strlen(filename);
    const size_t idLength = prefixLength + filenameLength + (version ? 0 : 2);
    if (!nameLength || idLength >= 20)
        return false;
    memcpy(gameId, "cdrom:", prefixLength);
    memcpy(gameId + prefixLength, filename, filenameLength);
    if (!version)
        memcpy(gameId + prefixLength + filenameLength, ";1", 2);
    gameId[idLength] = '\0';
    return true;
}
