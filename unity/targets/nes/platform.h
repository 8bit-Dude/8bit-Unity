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

// Memory Allocation (XRAM
#define BITMAPRAM   0x0300	// Stores RLE data
#define CHARMAPRAM  0x1000	// Stores charmap data
#define CHARFLAGRAM 0x0084	// Stores charflag+palette data
#define DECODERAM   0x0400	// Decoded char data
#define CHUNKRAM    0x0400	// Stores chunk data

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

// Choice of colors to put in Palette
#define PAL_AQUA	0x31
#define PAL_BLACK   0x0F
#define PAL_BLUE	0x11
#define PAL_DBLUE	0x01
#define PAL_MBLUE	0x11
#define PAL_LBLUE	0x21
#define PAL_BROWN	0x17
#define PAL_CYAN	0x3C
#define PAL_GREEN	0x19
#define PAL_DGREEN	0x09
#define PAL_MGREEN	0x19
#define PAL_LGREEN	0x29
#define PAL_GREY	0x00
#define PAL_DGREY	0x2D
#define PAL_MGREY	0x00
#define PAL_LGREY	0x10
#define PAL_ORANGE	0x27
#define PAL_PINK	0x25
#define PAL_PURPLE	0x14
#define PAL_RED		0x16
#define PAL_DRED	0x06
#define PAL_MRED	0x16
#define PAL_LRED	0x26
#define PAL_WHITE	0x30
#define PAL_YELLOW	0x38

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

// Reading files from ROM (see files.c)
extern unsigned char fileNum, fileIndex, *fileNames[];    
extern unsigned int  fileSizes[];
unsigned int FileRead(const char* filename, unsigned char* dst);
void DirList(void);
