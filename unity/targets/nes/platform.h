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
#define CHUNKRAM    0x0300	// Stores chunk data
#define WIDGETRAM   0x0300	// Stores callback data

// Character Mode
#define TXT_COLS 32
#define TXT_ROWS 25

// Charmap Mode
#define TXT_COLS    32
#define TXT_ROWS    25
#define FONT_HEIGHT  8	

// Charmap Mode
#define CMP_COLS   32
#define CMP_ROWS   25	

// Bitmap Mode
#define BMP_COLS    256
#define BMP_ROWS    200
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

// Text printing colors (uses palettes 1-3)
#define BLACK  0
#define WHITE  0
#define GREY   0
#define YELLOW 0
#define BLUE   1
#define CYAN   1
#define PURPLE 1
#define RED    2
#define BROWN  2
#define ORANGE 2
#define PINK   2
#define GREEN  3
#define LGREEN 3
#define DGREEN 3

// Clock
#define TCK_PER_SEC	CLK_TCK

// Dummy functions
#define bordercolor   textcolor
#define bgcolor       textcolor
#define clrscr        ClearBitmap

// Display functions
extern unsigned char autoRefresh, palBG[16];
void __fastcall__ SetVramAttr(void);
void __fastcall__ SetVramName(void);
void __fastcall__ SetVramChar(unsigned char chr);
void __fastcall__ SetVramColor(unsigned char forcePush);

// ROM/RAM transfer functions
void memcpyBanked(unsigned char* dst, unsigned char* src, unsigned int len, unsigned char bank);

// Hires gfx functions (see blitXXX.s)
void __fastcall__ BlitCharmap(void);
