/*
 * ps1-bare-metal - (C) 2023 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * We saw how to load a single texture and display it in the last two examples.
 * Textures, however, are not always simple images displayed in their entirety:
 * sometimes they hold more than one image (e.g. all frames of a character's
 * animation in a 2D game) but are "cropped out" on the fly during rendering to
 * only draw a single frame at a time. These textures are known as spritesheets
 * and the PS1's GPU fully supports them, as it allows for arbitrary UV
 * coordinates to be used.
 *
 * This example is going to show how to implement a simple font system for text
 * rendering, since that's one of the most common use cases for spritesheets. We
 * are going to load a single texture containing all our font's characters, as
 * having hundreds of tiny textures for each character would be extremely
 * inefficient, and then use a lookup table to obtain the UV coordinates, width
 * and height of each character in a string.
 *
 * NOTE: in order to make the code easier to read, I have moved all the
 * GPU-related functions from previous examples to a separate source file.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ps1/gpucmd.h"
#include "ps1/registers.h"
#include "ps1/cdrom.h"
#include "psxproject/cdrom.h"
#include "psxproject/irq.h"
#include "gpu.h"
#include "controller.h"
#include "psxproject/system.h"
#include "psxproject/spu.h"
#include <stdlib.h>
#include "file_manager.h"
#include "counters.h"
#include "logging.h"
#include "menu_media.h"
#include "menu_protocol.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define LISTING_SIZE 2324
#define MAX_FILES 4096

#if DEBUG_MAIN
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while (0)
#endif

#define SFX_VOL	10922 // 2/3 of maximal volume

// In order to pick sprites (characters) out of our spritesheet, we need a table
// listing all of them (in ASCII order in this case) with their UV coordinates
// within the sheet as well as their dimensions. In this example we're going to
// hardcode the table, however in an actual game you may want to store this data
// in the same file as the image and palette data.
typedef struct
{
	uint8_t x, y, width, height;
} SpriteInfo;

static const SpriteInfo fontSprites[] = {
	{.x = 6, .y = 0, .width = 2, .height = 9},	   // !
	{.x = 12, .y = 0, .width = 4, .height = 9},	   // "
	{.x = 18, .y = 0, .width = 6, .height = 9},	   // #
	{.x = 24, .y = 0, .width = 6, .height = 9},	   // $
	{.x = 30, .y = 0, .width = 6, .height = 9},	   // %
	{.x = 36, .y = 0, .width = 6, .height = 9},	   // &
	{.x = 42, .y = 0, .width = 2, .height = 9},	   // '
	{.x = 48, .y = 0, .width = 3, .height = 9},	   // (
	{.x = 54, .y = 0, .width = 3, .height = 9},	   // )
	{.x = 60, .y = 0, .width = 4, .height = 9},	   // *
	{.x = 66, .y = 0, .width = 6, .height = 9},	   // +
	{.x = 72, .y = 0, .width = 3, .height = 9},	   // ,
	{.x = 78, .y = 0, .width = 6, .height = 9},	   // -
	{.x = 84, .y = 0, .width = 2, .height = 9},	   // .
	{.x = 90, .y = 0, .width = 6, .height = 9},	   // /
	{.x = 0, .y = 9, .width = 6, .height = 9},	   // 0
	{.x = 6, .y = 9, .width = 6, .height = 9},	   // 1
	{.x = 12, .y = 9, .width = 6, .height = 9},	   // 2
	{.x = 18, .y = 9, .width = 6, .height = 9},	   // 3
	{.x = 24, .y = 9, .width = 6, .height = 9},	   // 4
	{.x = 30, .y = 9, .width = 6, .height = 9},	   // 5
	{.x = 36, .y = 9, .width = 6, .height = 9},	   // 6
	{.x = 42, .y = 9, .width = 6, .height = 9},	   // 7
	{.x = 48, .y = 9, .width = 6, .height = 9},	   // 8
	{.x = 54, .y = 9, .width = 6, .height = 9},	   // 9
	{.x = 60, .y = 9, .width = 2, .height = 9},	   // :
	{.x = 66, .y = 9, .width = 3, .height = 9},	   // ;
	{.x = 72, .y = 9, .width = 6, .height = 9},	   // <
	{.x = 78, .y = 9, .width = 6, .height = 9},	   // =
	{.x = 84, .y = 9, .width = 6, .height = 9},	   // >
	{.x = 90, .y = 9, .width = 6, .height = 9},	   // ?
	{.x = 0, .y = 18, .width = 6, .height = 9},	   // @
	{.x = 6, .y = 18, .width = 6, .height = 9},	   // A
	{.x = 12, .y = 18, .width = 6, .height = 9},   // B
	{.x = 18, .y = 18, .width = 6, .height = 9},   // C
	{.x = 24, .y = 18, .width = 6, .height = 9},   // D
	{.x = 30, .y = 18, .width = 6, .height = 9},   // E
	{.x = 36, .y = 18, .width = 6, .height = 9},   // F
	{.x = 42, .y = 18, .width = 6, .height = 9},   // G
	{.x = 48, .y = 18, .width = 6, .height = 9},   // H
	{.x = 54, .y = 18, .width = 4, .height = 9},   // I
	{.x = 60, .y = 18, .width = 5, .height = 9},   // J
	{.x = 66, .y = 18, .width = 6, .height = 9},   // K
	{.x = 72, .y = 18, .width = 6, .height = 9},   // L
	{.x = 78, .y = 18, .width = 6, .height = 9},   // M
	{.x = 84, .y = 18, .width = 6, .height = 9},   // N
	{.x = 90, .y = 18, .width = 6, .height = 9},   // O
	{.x = 0, .y = 27, .width = 6, .height = 9},	   // P
	{.x = 6, .y = 27, .width = 6, .height = 9},	   // Q
	{.x = 12, .y = 27, .width = 6, .height = 9},   // R
	{.x = 18, .y = 27, .width = 6, .height = 9},   // S
	{.x = 24, .y = 27, .width = 6, .height = 9},   // T
	{.x = 30, .y = 27, .width = 6, .height = 9},   // U
	{.x = 36, .y = 27, .width = 6, .height = 9},   // V
	{.x = 42, .y = 27, .width = 6, .height = 9},   // W
	{.x = 48, .y = 27, .width = 6, .height = 9},   // X
	{.x = 54, .y = 27, .width = 6, .height = 9},   // Y
	{.x = 60, .y = 27, .width = 6, .height = 9},   // Z
	{.x = 66, .y = 27, .width = 3, .height = 9},   // [
	{.x = 72, .y = 27, .width = 6, .height = 9},   // Backslash
	{.x = 78, .y = 27, .width = 3, .height = 9},   // ]
	{.x = 84, .y = 27, .width = 4, .height = 9},   // ^
	{.x = 90, .y = 27, .width = 6, .height = 9},   // _
	{.x = 0, .y = 36, .width = 3, .height = 9},	   // `
	{.x = 6, .y = 36, .width = 6, .height = 9},	   // a
	{.x = 12, .y = 36, .width = 6, .height = 9},   // b
	{.x = 18, .y = 36, .width = 6, .height = 9},   // c
	{.x = 24, .y = 36, .width = 6, .height = 9},   // d
	{.x = 30, .y = 36, .width = 6, .height = 9},   // e
	{.x = 36, .y = 36, .width = 5, .height = 9},   // f
	{.x = 42, .y = 36, .width = 6, .height = 9},   // g
	{.x = 48, .y = 36, .width = 5, .height = 9},   // h
	{.x = 54, .y = 36, .width = 2, .height = 9},   // i
	{.x = 60, .y = 36, .width = 4, .height = 9},   // j
	{.x = 66, .y = 36, .width = 5, .height = 9},   // k
	{.x = 72, .y = 36, .width = 2, .height = 9},   // l
	{.x = 78, .y = 36, .width = 6, .height = 9},   // m
	{.x = 84, .y = 36, .width = 5, .height = 9},   // n
	{.x = 90, .y = 36, .width = 6, .height = 9},   // o
	{.x = 0, .y = 45, .width = 6, .height = 9},	   // p
	{.x = 6, .y = 45, .width = 6, .height = 9},	   // q
	{.x = 12, .y = 45, .width = 6, .height = 9},   // r
	{.x = 18, .y = 45, .width = 6, .height = 9},   // s
	{.x = 24, .y = 45, .width = 5, .height = 9},   // t
	{.x = 30, .y = 45, .width = 5, .height = 9},   // u
	{.x = 36, .y = 45, .width = 6, .height = 9},   // v
	{.x = 42, .y = 45, .width = 6, .height = 9},   // w
	{.x = 48, .y = 45, .width = 6, .height = 9},   // x
	{.x = 54, .y = 45, .width = 6, .height = 9},   // y
	{.x = 60, .y = 45, .width = 5, .height = 9},   // z
	{.x = 66, .y = 45, .width = 4, .height = 9},   // {
	{.x = 72, .y = 45, .width = 2, .height = 9},   // |
	{.x = 78, .y = 45, .width = 4, .height = 9},   // }
	{.x = 84, .y = 45, .width = 6, .height = 9},   // ~
	{.x = 90, .y = 45, .width = 6, .height = 9},   // Invalid character
	{.x = 0, .y = 54, .width = 6, .height = 9},	   //
	{.x = 6, .y = 54, .width = 6, .height = 9},	   //
	{.x = 12, .y = 54, .width = 4, .height = 9},   //
	{.x = 18, .y = 54, .width = 4, .height = 9},   //
	{.x = 24, .y = 54, .width = 6, .height = 9},   //
	{.x = 30, .y = 54, .width = 6, .height = 9},   //
	{.x = 36, .y = 54, .width = 6, .height = 9},   //
	{.x = 42, .y = 54, .width = 6, .height = 9},   //
	{.x = 0, .y = 63, .width = 7, .height = 9},	   //
	{.x = 12, .y = 63, .width = 7, .height = 9},   //
	{.x = 24, .y = 63, .width = 9, .height = 9},   //
	{.x = 36, .y = 63, .width = 8, .height = 10},  //
	{.x = 48, .y = 63, .width = 11, .height = 10}, //
	{.x = 60, .y = 63, .width = 12, .height = 10}, //
	{.x = 72, .y = 63, .width = 14, .height = 9},  //
	{.x = 0, .y = 73, .width = 10, .height = 10},  //
	{.x = 12, .y = 73, .width = 10, .height = 10}, //
	{.x = 24, .y = 73, .width = 10, .height = 10}, //
	{.x = 36, .y = 73, .width = 10, .height = 9},  //
	{.x = 48, .y = 73, .width = 10, .height = 9},  //
	{.x = 60, .y = 73, .width = 10, .height = 10},  //
	{.x = 72, .y = 73, .width = 10, .height = 10},  //
	{.x = 85, .y = 73, .width = 8, .height = 8}  //
};

typedef enum
{
	MENU_COMMAND_NONE = 0x0,
	MENU_COMMAND_GOTO_ROOT = 0x1,
	MENU_COMMAND_GOTO_PARENT = 0x2,
	MENU_COMMAND_GOTO_DIRECTORY = 0x3,
	MENU_COMMAND_MOUNT_FILE_FAST = 0x4,
	MENU_COMMAND_MOUNT_FILE_SLOW = 0x5,
	MENU_COMMAND_REFRESH
} MENU_COMMAND;

typedef enum
{
	COMMAND_GOTO_ROOT = 0x1,
	COMMAND_GOTO_PARENT = 0x2,
	COMMAND_GOTO_DIRECTORY = 0x3,
	COMMAND_GET_NEXT_CONTENTS = 0x4,
	COMMAND_MOUNT_FILE = 0x5,
	COMMAND_EXTENDED = 0x8
} COMMAND;

#define FONT_FIRST_TABLE_CHAR '!'
#define FONT_SPACE_WIDTH 4
#define FONT_TAB_WIDTH 32
#define FONT_LINE_HEIGHT 10

static const SpriteInfo *getFontSprite(uint8_t ch)
{
	if (ch < FONT_FIRST_TABLE_CHAR || ch >= FONT_FIRST_TABLE_CHAR + sizeof(fontSprites) / sizeof(fontSprites[0]))
		ch = '\x7f';
	return &fontSprites[ch - FONT_FIRST_TABLE_CHAR];
}

static int getFilenameWidth(const char *name)
{
	int width = 0;
	for (; *name; ++name)
		width += *name == ' ' ? FONT_SPACE_WIDTH : getFontSprite((uint8_t)*name)->width;
	return width;
}

static void printStringClipped(
	DMAChain *chain, const TextureInfo *font, int x, int y, const char *str, int rightEdge)
{
	int currentX = x, currentY = y;

	uint32_t *ptr;

	// Start by sending a texpage command to tell the GPU to use the font's
	// spritesheet. Note that the texpage command before a drawing command can
	// be omitted when reusing the same texture, so sending it here just once is
	// enough.
	ptr = allocatePacket(chain, 1);
	ptr[0] = gp0_texpage(font->page, false, false);

	// Iterate over every character in the string.
	for (; *str; str++)
	{
		uint8_t ch = (uint8_t)*str;

		// Check if the character is "special" and shall be handled without
		// drawing any sprite, or if it's invalid and should be rendered as a
		// box with a question mark (character code 127).
		switch (ch)
		{
		case '\t':
			currentX += FONT_TAB_WIDTH - 1;
			currentX -= currentX % FONT_TAB_WIDTH;
			continue;

		case '\n':
			currentX = x;
			currentY += FONT_LINE_HEIGHT;
			continue;

		case ' ':
			currentX += FONT_SPACE_WIDTH;
			continue;
		}
		// If the character was not a tab, newline or space, fetch its
		// respective entry from the sprite coordinate table.
		const SpriteInfo *sprite = getFontSprite(ch);
		if (currentX + sprite->width > rightEdge)
			break;

		// Draw the character, summing the UV coordinates of the spritesheet in
		// VRAM to those of the sprite itself within the sheet. Enable blending
		// to make sure any semitransparent pixels in the font get rendered
		// correctly.
		ptr = allocatePacket(chain, 4);
		ptr[0] = gp0_rectangle(true, true, true);
		ptr[1] = gp0_xy(currentX, currentY);
		ptr[2] = gp0_uv(font->u + sprite->x, font->v + sprite->y, font->clut);
		ptr[3] = gp0_xy(sprite->width, sprite->height);

		// Move onto the next character.
		currentX += sprite->width;
	}
}

static void printString(DMAChain *chain, const TextureInfo *font, int x, int y, const char *str)
{
	printStringClipped(chain, font, x, y, str, 320);
}

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_WIDTH 96
#define FONT_HEIGHT 84
#define FONT_COLOR_DEPTH GP0_COLOR_4BPP

#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 20
#define TEXTURE_COLOR_DEPTH GP0_COLOR_4BPP

enum {
	BOOT_FAST,
	BOOT_REGULAR,
	BOOT_CHEATS,
	BOOT_OVERCLOCK,
	BOOT_EDC,
	BOOT_SECTOR_FIX,
	BOOT_OPTION_COUNT
};

static void drawMenuRectangle(DMAChain *chain, uint32_t color, int x, int y, int width, int height)
{
	uint32_t *ptr = allocatePacket(chain, 3);
	ptr[0] = color | gp0_rectangle(false, false, false);
	ptr[1] = gp0_xy(x, y);
	ptr[2] = gp0_xy(width, height);
}

static void drawMenuPanel(DMAChain *chain, const MenuColors *colors, int x, int y, int width, int height)
{
	drawMenuRectangle(chain, colors->frame, x, y, width, height);
	drawMenuRectangle(chain, colors->background, x + 1, y + 1, width - 2, height - 2);
}

static void drawMenuAction(DMAChain *chain, const TextureInfo *font, int x, int y,
                           unsigned button, const char *label)
{
	if (button == BUTTON_MASK_SELECT)
	{
		drawMenuRectangle(chain, gp0_rgb(136, 144, 152), x + 1, y + 2, 9, 6);
		drawMenuRectangle(chain, gp0_rgb(84, 92, 100), x + 2, y + 2, 7, 4);
	}
	else
	{
		const uint32_t color = button == BUTTON_MASK_X ? gp0_rgb(156, 195, 215) : gp0_rgb(240, 156, 164);
		drawMenuRectangle(chain, color, x + 2, y, 6, 10);
		drawMenuRectangle(chain, color, x + 1, y + 1, 8, 8);
		drawMenuRectangle(chain, color, x, y + 2, 10, 6);
		const char *mark = button == BUTTON_MASK_X ? "x" : "o";
		printString(chain, font, x + 2, y, mark);
	}
	printString(chain, font, x + 14, y, label);
}

static void drawBootOptions(DMAChain *chain, const TextureInfo *font, const MenuColors *colors,
                            unsigned selected, bool edcCorrection, bool bootsectorFix)
{
	const char *const options[] = {
	    "Fast Boot", "Regular Boot", "Cheats", "Overclock", "EDC correction", "Bootsector Fix"
	};
	drawMenuPanel(chain, colors, 18, 24, 154, 100);
	drawMenuRectangle(chain, colors->frame, 24, 28, 142, 74);
	drawMenuRectangle(chain, colors->fileList, 25, 29, 140, 72);
	drawMenuRectangle(chain, gp0_rgb(224, 24, 24), 25, 29 + 12 * selected, 140, 12);
	for (unsigned i = 0; i < BOOT_OPTION_COUNT; ++i)
	{
		const int y = 30 + 12 * i;
		if (i >= BOOT_OVERCLOCK)
		{
			const bool enabled = i == BOOT_OVERCLOCK || (i == BOOT_EDC ? edcCorrection : bootsectorFix);
			drawMenuRectangle(chain, enabled ? gp0_rgb(64, 200, 48) : gp0_rgb(96, 128, 168),
			                  26, y + 3, 11, 5);
			drawMenuRectangle(chain, gp0_rgb(240, 240, 240), enabled ? 32 : 26, y + 3, 5, 5);
		}
		printString(chain, font, 40, y, options[i]);
	}
	drawMenuAction(chain, font, 32, 108, BUTTON_MASK_X, "OK");
	drawMenuAction(chain, font, 82, 108, BUTTON_MASK_CIRCLE, "Back");
}

static void drawBootProgress(DMAChain *chain, const TextureInfo *font, const MenuColors *colors,
                             const char *const *lines, unsigned count)
{
	const int height = 14 + 12 * count;
	drawMenuPanel(chain, colors, 16, 24, 190, height);
	drawMenuRectangle(chain, colors->frame, 20, 28, 182, height - 8);
	drawMenuRectangle(chain, colors->fileList, 21, 29, 180, height - 10);
	for (unsigned i = 0; i < count; ++i)
		printStringClipped(chain, font, 25, 32 + 12 * i, lines[i], 198);
}

static void showBootProgress(DMAChain *chain, uint32_t *overlay, const TextureInfo *font,
                             const MenuColors *colors, const char *const *lines, unsigned count)
{
	waitForDMADone();
	chain->nextPacket = overlay;
	drawBootProgress(chain, font, colors, lines, count);
	*chain->nextPacket = gp0_endTag(0);
	waitForGP0Ready();
	waitForVblank();
	sendLinkedList(chain->data);
}

extern const uint8_t fontTexture[], fontPalette[], logoTexture[], logoPalette[];
extern const uint8_t click_sfx[], slide_sfx[];

#define c_maxFilePathLength 255
#define c_maxFilePathLengthWithTerminator c_maxFilePathLength + 1
#define c_maxFileEntriesPerSector 8

int loadchecker = 0;

void wait_ms(uint32_t ms)
{
	uint32_t frequency = (GPU_GP1 & GP1_STAT_FB_MODE_BITMASK) == GP1_STAT_FB_MODE_PAL ? 50 : 60;
	uint32_t frames = (ms * frequency) / 1000;
	for (uint32_t i = 0; i < frames; ++i)
	{
		waitForVblank();
	}
}

static MenuListingStatus readListingPage(uint16_t *itemCount, char *page)
{
    const uint8_t *data = (const uint8_t *)page;
    uint16_t offset = 0;
    while (offset + 2 <= LISTING_SIZE && *itemCount < MAX_FILES)
    {
        const uint16_t length = data[offset];
        if (length == 0)
        {
            if (offset + 4 > LISTING_SIZE || data[offset + 1] > MENU_LIST_OPEN_FAILED)
                return MENU_LIST_READ_FAILED;
            const MenuListingStatus status = (MenuListingStatus)data[offset + 1];
            if (status <= MENU_LIST_MORE && !data[offset + 2] && !data[offset + 3])
                return MENU_LIST_READ_FAILED;
            return status;
        }
        if (offset + 2 + length > LISTING_SIZE || data[offset + 1] > 1)
            return MENU_LIST_READ_FAILED;
        file_manager_init_file_data(*itemCount, data[offset + 1], page + offset + 2, length);
        offset += length + 2;
        ++*itemCount;
    }

    return *itemCount == MAX_FILES ? MENU_LIST_DONE : MENU_LIST_READ_FAILED;
}

static uint32_t list_load(void *sectorBuffer, uint8_t command, uint16_t argument, MenuListingStatus *status)
{
    uint16_t fileEntryCount = 0;

    do
    {
        memset(sectorBuffer, 0, MENU_RAW_SECTOR_BYTES);
        sendMenuCommand(command, argument);
        startCDROMRead(100, sectorBuffer, 1, MENU_RAW_SECTOR_BYTES, true, true);
        if (!waitingForInt5)
        {
            *status = MENU_LIST_READ_FAILED;
            return 0;
        }

        const uint16_t previousCount = fileEntryCount;
        *status = readListingPage(&fileEntryCount, (char *)sectorBuffer + MENU_USER_DATA_OFFSET);
        if (*status == MENU_LIST_MORE && fileEntryCount == previousCount)
            *status = MENU_LIST_READ_FAILED;
        if (*status > MENU_LIST_MORE)
            return 0;
        command = COMMAND_GET_NEXT_CONTENTS;
        argument = fileEntryCount;
    } while (*status == MENU_LIST_MORE);

    file_manager_sort(fileEntryCount);
    return fileEntryCount;
}

int main(int argc, const char **argv)
{
	COUNTERS[1].mode = 0x0100;

	initIRQ();
#if DEBUG_LOGGING_ENABLED
	initSerialIO(115200);
#endif
	initControllerBus();
	initCDROM();
	initSPU();
	
	static Sound sfx_click;
	static Sound sfx_slide;
	
	sound_loadSoundFromBinary(click_sfx, &sfx_click);
	sound_loadSoundFromBinary(slide_sfx, &sfx_slide);
	
	file_manager_init();

	uint8_t currentCommand = MENU_COMMAND_GOTO_ROOT;

	DEBUG_PRINT("Hello from menu loader!\n");

	if ((GPU_GP1 & GP1_STAT_FB_MODE_BITMASK) == GP1_STAT_FB_MODE_PAL)
	{
		DEBUG_PRINT("Using PAL mode\n");
		setupGPU(GP1_MODE_PAL, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else
	{
		DEBUG_PRINT("Using NTSC mode\n");
		setupGPU(GP1_MODE_NTSC, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_GPU);

	GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
	GPU_GP1 = gp1_dispBlank(false);

	TextureInfo font;
	TextureInfo logo;

	uploadIndexedTexture(
		&font, fontTexture, fontPalette, SCREEN_WIDTH * 2, 0, SCREEN_WIDTH * 2,
		FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, FONT_COLOR_DEPTH
	);

	uploadIndexedTexture(
		&logo, logoTexture, logoPalette, 736, 0,
		736, TEXTURE_HEIGHT, TEXTURE_WIDTH, TEXTURE_HEIGHT,
		TEXTURE_COLOR_DEPTH
	);

	DMAChain dmaChains[2];
	bool usingSecondFrame = false;

	uint32_t sectorBuffer[MENU_RAW_SECTOR_BYTES / sizeof(uint32_t)];
	TextureInfo cover;
	MenuColors colors = {
		.background = gp0_rgb(0, 18, 56),
		.frame = gp0_rgb(255, 255, 255),
		.fileList = gp0_rgb(0, 48, 128)
	};
	loadMenuColors(&colors);
	
	int highlight = 72;
	int highlightStep = 1;
	unsigned filenameOffset = 0;
	int filenameStep = 1;
	unsigned filenameDelay = 30;
	
	uint32_t fileEntryCount = 0;
	MenuListingStatus listingStatus = MENU_LIST_DONE;

	uint16_t selectedindex = 0;
	uint16_t parentSelections[c_maxFilePathLength];
	unsigned directoryDepth = 0;

	int creditsmenu = 0;
	bool bootMenu = false;
	unsigned bootOption = BOOT_FAST;
	const char *bootNotice = NULL;
	bool edcCorrection = true;
	bool bootsectorFix = false;
	unsigned errorFrames = 0;

	uint16_t previousButtons = getButtonPress(0);

	for (;;)
	{
		int bufferX = usingSecondFrame ? SCREEN_WIDTH : 0;
		int bufferY = 0;

		DMAChain *chain = &dmaChains[usingSecondFrame];
		usingSecondFrame = !usingSecondFrame;

		uint32_t *ptr;
		uint32_t *bootOverlay = NULL;
		char mountLine[MAX_FILE_LENGTH + sizeof("Mount ")];
		const char *bootProgress[8] = { "Please Wait Loading...", mountLine, "Update TOC", "Check CD",
		                                "load SYSTEM.CNF" };
		
		GPU_GP1 = gp1_fbOffset(bufferX, bufferY);

		chain->nextPacket = chain->data;

		ptr = allocatePacket(chain, 4);
		ptr[0] = gp0_texpage(0, true, false);
		ptr[1] = gp0_fbOffset1(bufferX, bufferY);
		ptr[2] = gp0_fbOffset2(bufferX + SCREEN_WIDTH - 1, bufferY + SCREEN_HEIGHT - 2);
		ptr[3] = gp0_fbOrigin(bufferX, bufferY);

		ptr    = allocatePacket(chain, 3);
		ptr[0] = colors.background | gp0_vramFill();
		ptr[1] = gp0_xy(bufferX, bufferY);
		ptr[2] = gp0_xy(SCREEN_WIDTH, SCREEN_HEIGHT);

		ptr = allocatePacket(chain, 6);
		ptr[0] = colors.frame | gp0_rectangle(false, false, false);
		ptr[1] = gp0_xy(8, 14);
		ptr[2] = gp0_xy(174, 202);
		ptr[3] = colors.fileList | gp0_rectangle(false, false, false);
		ptr[4] = gp0_xy(9, 15);
		ptr[5] = gp0_xy(172, 200);

		ptr = allocatePacket(chain, 5);
		ptr[0] = gp0_texpage(logo.page, false, false);
		ptr[1] = gp0_rectangle(true, true, true);
		ptr[2] = gp0_xy(185, 14);
		ptr[3] = gp0_uv(logo.u, logo.v, logo.clut);
		ptr[4] = gp0_xy(logo.width, logo.height);
		
		// get the controller button press
		uint16_t buttons = getButtonPress(0);
		uint16_t pressedButtons = ~previousButtons & buttons;
		static uint8_t hold = 0;
		
		if ((buttons & BUTTON_MASK_UP) && (previousButtons & BUTTON_MASK_UP))
		{
			if (++hold > 30) {
				pressedButtons ^= BUTTON_MASK_UP;
				hold = 25;
			}
		}
		else if ((buttons & BUTTON_MASK_DOWN) && (previousButtons & BUTTON_MASK_DOWN))
		{
			if (++hold > 30) {
				pressedButtons ^= BUTTON_MASK_DOWN;
				hold = 25;
			}
		}
		else {
			hold = 0;
		}

		const uint16_t pageSize = 15;
		const int rowHeight = 13;

		if (!bootMenu && (pressedButtons & BUTTON_MASK_SELECT))
		{
			creditsmenu = creditsmenu == 0 ? 1 : 0;
		}

		if (errorFrames)
            pressedButtons = 0;

		if (creditsmenu == 0)
		{
			const uint16_t previousSelection = selectedindex;
			if (bootMenu)
			{
				if (pressedButtons & BUTTON_MASK_CIRCLE)
				{
					if (bootNotice)
						bootNotice = NULL;
					else
						bootMenu = false;
				}
				else if (bootNotice)
				{
					if (pressedButtons & BUTTON_MASK_X)
						bootNotice = NULL;
				}
				else if (pressedButtons & (BUTTON_MASK_UP | BUTTON_MASK_DOWN))
				{
					const unsigned step = pressedButtons & BUTTON_MASK_UP ? BOOT_OPTION_COUNT - 1 : 1;
					bootOption = (bootOption + step) % BOOT_OPTION_COUNT;
					sound_playOnChannel(&sfx_click, SFX_VOL, SFX_VOL, 0);
				}
				else if (pressedButtons & BUTTON_MASK_X)
				{
					switch (bootOption)
					{
					case BOOT_FAST:
					case BOOT_REGULAR:
						currentCommand = bootOption == BOOT_FAST ? MENU_COMMAND_MOUNT_FILE_FAST :
						                                           MENU_COMMAND_MOUNT_FILE_SLOW;
						bootMenu = false;
						break;
					case BOOT_CHEATS:
						bootNotice = "Cheats unavailable";
						break;
					case BOOT_OVERCLOCK:
						bootNotice = "Clock set by firmware";
						break;
					case BOOT_EDC:
						edcCorrection = !edcCorrection;
						break;
					case BOOT_SECTOR_FIX:
						bootsectorFix = !bootsectorFix;
						break;
					}
					sound_playOnChannel(&sfx_slide, SFX_VOL, SFX_VOL, 1);
				}
			}
			else
			{
				if (fileEntryCount == 0)
				{
					pressedButtons &= ~(BUTTON_MASK_UP | BUTTON_MASK_DOWN | BUTTON_MASK_LEFT | BUTTON_MASK_RIGHT |
					                    BUTTON_MASK_L1 | BUTTON_MASK_R1);
				}
				if (pressedButtons & BUTTON_MASK_UP)
				{
					selectedindex = selectedindex > 0 ? selectedindex - 1 : fileEntryCount - 1;
				}
				else if (pressedButtons & BUTTON_MASK_DOWN)
				{
					selectedindex = selectedindex < (int)(fileEntryCount - 1) ? selectedindex + 1 : 0;
				}
			
				if (pressedButtons & (BUTTON_MASK_LEFT | BUTTON_MASK_L1))
				{
					selectedindex = selectedindex >= pageSize ? selectedindex - pageSize : 0;
				}
				else if (pressedButtons & (BUTTON_MASK_RIGHT | BUTTON_MASK_R1))
				{
					selectedindex = selectedindex < (int)(fileEntryCount - (pageSize + 1)) ? selectedindex + pageSize : fileEntryCount - 1;
				}
			
				if (pressedButtons & (BUTTON_MASK_UP | BUTTON_MASK_DOWN | BUTTON_MASK_LEFT | BUTTON_MASK_RIGHT 
																		| BUTTON_MASK_L1   | BUTTON_MASK_R1))
				{
					sound_playOnChannel(&sfx_click, SFX_VOL, SFX_VOL, 0);
				}

				if ((pressedButtons & BUTTON_MASK_X) && fileEntryCount)
				{
					sound_playOnChannel(&sfx_slide, SFX_VOL, SFX_VOL, 1);
					if (file_manager_get_file_data(selectedindex)->flag)
						currentCommand = MENU_COMMAND_GOTO_DIRECTORY;
					else
					{
						bootOption = BOOT_FAST;
						bootNotice = NULL;
						bootMenu = true;
					}
				}
				else if ((pressedButtons & BUTTON_MASK_X) && listingStatus != MENU_LIST_DONE)
				{
					currentCommand = MENU_COMMAND_REFRESH;
				}
				else if ((pressedButtons & BUTTON_MASK_CIRCLE) && directoryDepth)
				{
					currentCommand = MENU_COMMAND_GOTO_PARENT;
					sound_playOnChannel(&sfx_slide, SFX_VOL, SFX_VOL, 1);
				}
			}

			const bool mounting = currentCommand == MENU_COMMAND_MOUNT_FILE_FAST ||
			                      currentCommand == MENU_COMMAND_MOUNT_FILE_SLOW;

            if (errorFrames)
            {
                printString(chain, &font, 20, 70, "Unable to load game.");
                printString(chain, &font, 20, 90, "Check CUE/BIN and SD card.");
                printString(chain, &font, 20, 110, "Use a raw 2352-byte PS1 image.");
                --errorFrames;
            }
            else if (currentCommand != MENU_COMMAND_NONE && !mounting)
            {
                printString(chain, &font, 40, 40, "Please Wait Loading...");
			}
			else
			{
				if (buttons || selectedindex != previousSelection)
				{
					filenameOffset = 0;
					filenameStep = 1;
					filenameDelay = 30;
				}
				else if (filenameDelay)
				{
					--filenameDelay;
				}

				char fbuffer[32];
				snprintf(fbuffer, sizeof(fbuffer), "%u of %u", fileEntryCount ? selectedindex + 1u : 0u,
				         fileEntryCount);
				printString(chain, &font, 186, 66, fbuffer);

				if (fileEntryCount && updateMenuCover(file_manager_get_file_index(selectedindex), &cover))
				{
					ptr = allocatePacket(chain, 5);
					ptr[0] = gp0_texpage(cover.page, false, false);
					ptr[1] = gp0_rectangle(true, true, false);
					ptr[2] = gp0_xy(185, 82);
					ptr[3] = gp0_uv(cover.u, cover.v, cover.clut);
					ptr[4] = gp0_xy(cover.width, cover.height);
				}

				int32_t start = 0;
				if ((int32_t)fileEntryCount >= pageSize)
				{
					start = MIN(MAX(selectedindex - (pageSize / 2), 0), (int32_t)fileEntryCount - pageSize);
				}

				int32_t itemCount = MIN(start + pageSize, (int32_t)fileEntryCount) - start;
				if (itemCount > 0)
				{
					for (int32_t i = 0; i < itemCount; i++)
					{
						uint32_t index = start + i;
						const fileData *file = file_manager_get_file_data(index);
						const char icon = file->flag == 0 ? '\x8f' : '\x92';
						const int iconWidth = getFontSprite((uint8_t)icon)->width + FONT_SPACE_WIDTH;

						if (index == selectedindex)
						{
							const int listWidth = 172;
							const int nameWidth = getFilenameWidth(file->filename);
							if (iconWidth + nameWidth + 2 > 174 && filenameDelay == 0)
							{
								filenameOffset += filenameStep;
								filenameDelay = 15;
								if (filenameOffset == 0 || (filenameStep > 0 &&
								    iconWidth + getFilenameWidth(file->filename + filenameOffset) < 168))
								{
									filenameStep = -filenameStep;
									filenameDelay = 30;
								}
							}

							ptr = allocatePacket(chain, 3);
							ptr[0] = gp0_rgb(highlight, 0, 0) | gp0_rectangle(false, false, false);
							ptr[1] = gp0_xy(9, 15 + i * rowHeight);
							ptr[2] = gp0_xy(MIN(listWidth, iconWidth + nameWidth + 2), 12);
						}

						char buffer[MAX_FILE_LENGTH + 3];
						const unsigned offset = index == selectedindex ? filenameOffset : 0;
						snprintf(buffer, sizeof(buffer), "%c %s", icon, file->filename + offset);
						printStringClipped(chain, &font, 10, 17 + i * rowHeight, buffer, 180);
					}
				}
				else
				{
					const char *message = "Empty Folder";
					if (listingStatus == MENU_LIST_NO_SD)
						message = "No SD Card";
					else if (listingStatus == MENU_LIST_OPEN_FAILED)
						message = "Unable to open folder";
					else if (listingStatus == MENU_LIST_READ_FAILED)
						message = "Unable to read folder";
					printString(chain, &font, 40, 40, message);
				}

				if (mounting)
				{
					snprintf(mountLine, sizeof(mountLine), "Mount %s",
					         file_manager_get_file_data(selectedindex)->filename);
					bootOverlay = chain->nextPacket;
					drawBootProgress(chain, &font, &colors, bootProgress, 2);
				}
				else if (bootMenu)
				{
					drawBootOptions(chain, &font, &colors, bootOption, edcCorrection, bootsectorFix);
					if (bootNotice)
					{
						drawMenuPanel(chain, &colors, 24, 56, 140, 23);
						printString(chain, &font, 30, 63, bootNotice);
					}
				}
				else
				{
					if (fileEntryCount || listingStatus != MENU_LIST_DONE)
						drawMenuAction(chain, &font, 12, 218, BUTTON_MASK_X, fileEntryCount ? "Open" : "Retry");
					drawMenuAction(chain, &font, 62, 218, BUTTON_MASK_SELECT, "Credits");
					if (directoryDepth)
						drawMenuAction(chain, &font, 156, 218, BUTTON_MASK_CIRCLE, "Back");
				}

				highlight += highlightStep;
				if (highlight == 224 || highlight == 72)
					highlightStep = -highlightStep;
			}
		}
		else
		{
			printString(
				chain, &font, 40, 40,
				"PicoStation EX 1.0.0");
			printString(
				chain, &font, 40, 80,
				"Alex Ishida");

			printString(
				chain, &font, 40, 120,
				"Original project by megavolt85");

		}


		previousButtons = buttons;
		*(chain->nextPacket) = gp0_endTag(0);
		waitForGP0Ready();
		waitForVblank();
		sendLinkedList(chain->data);

		if (currentCommand != MENU_COMMAND_NONE)
		{
			resetMenuCover();
			filenameOffset = 0;
			filenameStep = 1;
			filenameDelay = 30;
			if (currentCommand == MENU_COMMAND_GOTO_ROOT)
			{
				fileEntryCount = list_load(sectorBuffer, COMMAND_GOTO_ROOT, 0, &listingStatus);
			}
			else if (currentCommand == MENU_COMMAND_GOTO_PARENT)
			{
				fileEntryCount = list_load(sectorBuffer, COMMAND_GOTO_PARENT, 0, &listingStatus);
				const uint16_t parentSelection = parentSelections[--directoryDepth];
				selectedindex = fileEntryCount ? MIN(parentSelection, fileEntryCount - 1) : 0;
			}
			else if (currentCommand == MENU_COMMAND_GOTO_DIRECTORY)
			{
				uint16_t index = file_manager_get_file_index(selectedindex);
				fileEntryCount = list_load(sectorBuffer, COMMAND_GOTO_DIRECTORY, index, &listingStatus);
				if (listingStatus != MENU_LIST_OPEN_FAILED &&
				    directoryDepth < sizeof(parentSelections) / sizeof(parentSelections[0]))
					parentSelections[directoryDepth++] = selectedindex;
				selectedindex = 0;
			}
			else if (currentCommand == MENU_COMMAND_REFRESH)
			{
				fileEntryCount = list_load(sectorBuffer, COMMAND_GET_NEXT_CONTENTS, 0, &listingStatus);
				selectedindex = 0;
			}
			else if ((currentCommand == MENU_COMMAND_MOUNT_FILE_FAST) || (currentCommand == MENU_COMMAND_MOUNT_FILE_SLOW))
			{
				DEBUG_PRINT("DEBUG: selectedindex :%d\n", selectedindex);

				uint16_t index = file_manager_get_file_index(selectedindex);
                // Keep the menu mounted until the firmware validates the image.
                memset(sectorBuffer, 0, MENU_RAW_SECTOR_BYTES);
                sendMenuCommand(MENU_PREPARE_COMMAND, index);
                delayMicroseconds(400000);
                startCDROMRead(MENU_MOUNT_STATUS_LBA, sectorBuffer, 1,
                               MENU_RAW_SECTOR_BYTES, true, true);
                const uint8_t* status = (const uint8_t*)sectorBuffer + MENU_USER_DATA_OFFSET;
                if (memcmp(status, "MNT1", 4) ||
                    (status[4] | (uint16_t)status[5] << 8) != index ||
                    status[6] || status[7])
                {
                    errorFrames = 240;
                    currentCommand = MENU_COMMAND_NONE;
                    continue;
                }
                const bool audioOnly = status[MENU_MOUNT_FLAGS_OFFSET] & MENU_MOUNT_AUDIO_ONLY;
                sendMenuCommand(COMMAND_MOUNT_FILE, index);
                delayMicroseconds(400000);
                showBootProgress(chain, bootOverlay, &font, &colors, bootProgress, 3);
                updateCDROM_TOC();
                delayMicroseconds(400000);
                sendMenuCommand(COMMAND_EXTENDED, (MENU_EXTENDED_SKIP_BOOTSECTOR << 1) | !bootsectorFix);
                waitForINT3();
                sendMenuCommand(COMMAND_EXTENDED, (MENU_EXTENDED_SKIP_EDC << 1) | !edcCorrection);
                waitForINT3();
                showBootProgress(chain, bootOverlay, &font, &colors, bootProgress, 4);

                if (audioOnly)
                    softReset();

                if (!is_playstation_cd())
                {
                    if (waitingForInt5)
                        softReset();

                    sendMenuCommand(MENU_RETURN_COMMAND, 0);
                    delayMicroseconds(400000);
                    errorFrames = 240;
                    currentCommand = MENU_COMMAND_NONE;
                    continue;
                }

                showBootProgress(chain, bootOverlay, &font, &colors, bootProgress, 5);
                char gameId[20];
                char bootPath[40];
                unsigned progressCount = 5;
                char bootLine[sizeof("BOOT = ") + sizeof(bootPath)];
                char gameIdLine[sizeof("Send ID ") + sizeof(gameId)];
                if (getMenuGameId(sectorBuffer, gameId, bootPath))
                {
                    snprintf(bootLine, sizeof(bootLine), "BOOT = %s", bootPath);
                    snprintf(gameIdLine, sizeof(gameIdLine), "Send ID %s", gameId);
                    bootProgress[progressCount++] = bootLine;
                    bootProgress[progressCount++] = gameIdLine;
                    showBootProgress(chain, bootOverlay, &font, &colors, bootProgress, progressCount);
                    sendGameID(gameId, 3);
                }
                bootProgress[progressCount++] = "Load and execute";
                showBootProgress(chain, bootOverlay, &font, &colors, bootProgress, progressCount);
                waitForDMADone();
                if (currentCommand == MENU_COMMAND_MOUNT_FILE_FAST)
                    softFastReboot();
                else
                    softReset();
			}

			currentCommand = MENU_COMMAND_NONE;
		}
	}

	return 0;
}
