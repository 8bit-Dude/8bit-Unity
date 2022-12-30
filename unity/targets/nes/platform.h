/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/05/17
 *	
 * Copyright (c) 2021 Anthony Beaucamp.
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
 *
 *	Credits: 
 *		* Oliver Schmidt for his IP65 network interface
 *		* Christian Groessler for helping optimize the memory maps on Commodore and Atari
 *		* Bill Buckels for his Apple II Double Hi-Res bitmap code
 */
 
#include "neslib.h"
#include "MMC1/bank_helpers.h"

// Memory Allocation
#define BITMAPRAM   0x0300	// Stores RLE data
#define CHARMAPRAM  0x1000	// Stores charmap data
#define CHARFLAGRAM 0x0084	// Stores charflag+palette data
#define DECODERAM   0x0400	// Decoded char data
#define WIDGETRAM   0x0200	// Stores callback data

// Character Mode
#define TXT_COLS 	32
#define TXT_ROWS 	24
#define FONT_WIDTH  1
#define FONT_HEIGHT 1

// Charmap Mode
#define CMP_COLS   32
#define CMP_ROWS   24	

// Bitmap Mode
#define BMP_COLS    256
#define BMP_ROWS    192
#define BMP_PALETTE   4

// Colors available for Sprite Palette
#define SPR_VOID    0x00
#define SPR_AQUA	0x31
#define SPR_BLACK   0x0F
#define SPR_BLUE	0x11
#define SPR_DBLUE	0x01
#define SPR_MBLUE	0x11
#define SPR_LBLUE	0x21
#define SPR_BROWN	0x17
#define SPR_CYAN	0x3C
#define SPR_GREEN	0x19
#define SPR_DGREEN	0x09
#define SPR_MGREEN	0x19
#define SPR_LGREEN	0x29
#define SPR_GREY	0x00
#define SPR_DGREY	0x2D
#define SPR_MGREY	0x00
#define SPR_LGREY	0x10
#define SPR_ORANGE	0x27
#define SPR_PINK	0x25
#define SPR_PURPLE	0x14
#define SPR_RED		0x16
#define SPR_DRED	0x06
#define SPR_MRED	0x16
#define SPR_LRED	0x26
#define SPR_WHITE	0x30
#define SPR_YELLOW	0x38

// Text printing colors (uses palettes 0-3)
#define GREEN  0
#define LGREEN 0
#define DGREEN 0
#define BLUE   1
#define CYAN   1
#define PURPLE 1
#define RED    2
#define BROWN  2
#define ORANGE 2
#define PINK   2
#define YELLOW 3
#define GREY   3
#define WHITE  3
#define BLACK  7

// Workaround for missing chardefs
#undef CH_DEL
#undef CH_ENTER
#define CH_DEL   0x14
#define CH_ENTER 0x0D

// Clock
#define TCK_PER_SEC	tck_per_sec

// Dummy functions
#define bordercolor   textcolor
#define bgcolor       textcolor
#define clrscr        ClearBitmap

// Display functions
extern unsigned char tck_per_sec;
extern unsigned char autoRefresh, palBG[16];
extern unsigned char vram_attr[64];
extern unsigned char vram_attr_index;
void __fastcall__ SetVramAttr(void);
void __fastcall__ SetVramName(void);
void __fastcall__ SetVramChar(unsigned char chr);
void __fastcall__ SetVramColor(unsigned char forcePush);
void __fastcall__ FillVram(unsigned char* data);

// ROM/RAM transfer functions
void memcpyBanked(unsigned char* dst, unsigned char* src, unsigned int len, unsigned char bank);

// Hires gfx functions (see blitXXX.s)
void __fastcall__ BlitCharmap(void);

// Soft Keyboard functions
char cgetKeyboardOverlay(void);
