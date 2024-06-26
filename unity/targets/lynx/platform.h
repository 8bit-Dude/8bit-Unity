/*
 * Copyright (c) 2018 Anthony Beaucamp.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 */

#include <lynx.h>

// Memory Map
#define BITMAPRAM  (0x9F8B)
#define MUSICRAM   (BITMAPRAM - MUSICSIZE)
#define SHAREDRAM  (MUSICRAM - SHAREDSIZE)

// Text Mode
#define TXT_COLS   40
#define TXT_ROWS   17
#define FONT_HEIGHT 6	

// Charmap Mode
#define CMP_COLS 40
#define CMP_ROWS 25	
#define FONT_WIDTH  4
#define FONT_HEIGHT 6

// Bitmap Mode (Default)
#define BMP_COLS 160
#define BMP_ROWS 102
#define BMP_PALETTE 16

// Bitmap Palette
#define PURPLE 0
#define DBLUE  1
#define BLUE   2
#define CYAN   2
#define LBLUE  3
#define LGREEN 4
#define GREEN  5
#define DGREEN 6
#define BROWN  7
#define ORANGE 8
#define YELLOW 9
#define PINK   10
#define RED    11
#define DRED   12
#define WHITE  13
#define GREY   14
#define BLACK  15

// Workaround for missing chardefs
#define CH_DEL   0x14
#define CH_ENTER 0x0D

// Clock
#define TCK_PER_SEC	60

// Display management (see display.c)
void InitDisplay(void);
void __fastcall__ SuzyFlip(void);
extern unsigned char autoRefresh; // Toggle for automatic screen refresh after LoadBitmap(), PrintStr(), PrintNum(), PrintBlanks()

// Gfx functions (see blitCharmap.s)
void __fastcall__ BlitCharmap(void);

// Default Sprite Colors;
extern unsigned char defaultColors[];

// Soft Keyboard functions
char cgetKeyboardOverlay(void);
