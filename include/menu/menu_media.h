#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "gpu.h"

typedef struct {
    uint32_t background;
    uint32_t frame;
    uint32_t fileList;
} MenuColors;

void sendMenuCommand(uint8_t command, uint16_t argument);
bool getMenuGameId(void *sectorBuffer, char gameId[20], char executable[40]);
void loadMenuColors(MenuColors *colors);
bool updateMenuCover(uint16_t index, TextureInfo *texture);
void resetMenuCover(void);
