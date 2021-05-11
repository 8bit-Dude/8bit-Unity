/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2019/07/12
 *	
 * Copyright (c) 2019 Anthony Beaucamp.
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
#include "nesdoug.h" 
#include "MMC1/bank_helpers.h"

// Character Mode
#define TXT_COLS 32
#define TXT_ROWS 25

// Charmap Mode
#define TXT_COLS   32
#define TXT_ROWS   25
#define FONT_HEIGHT 8	

// Charmap Mode
#define CMP_COLS   32
#define CMP_ROWS   25	

// Bitmap Mode
#define BMP_COLS    256
#define BMP_ROWS    200
#define BMP_PALETTE 16

// Global Palette
#define AQUA	0x31
#define BLACK   0x0F
#define BLUE	0x11
#define DBLUE	0x01
#define MBLUE	0x11
#define LBLUE	0x21
#define BROWN	0x17
#define CYAN	0x3C
#define GREEN	0x19
#define DGREEN	0x09
#define MGREEN	0x19
#define LGREEN	0x29
#define GREY	0x00
#define DGREY	0x2D
#define MGREY	0x00
#define LGREY	0x10
#define ORANGE	0x27
#define PINK	0x25
#define PURPLE	0x14
#define RED		0x16
#define DRED	0x06
#define MRED	0x16
#define LRED	0x26
#define WHITE	0x30
#define YELLOW	0x38

// Clock
#define TCK_PER_SEC	CLK_TCK

// Dummy functions
#define UpdateDisplay ppu_wait_frame
#define bordercolor   textcolor
#define bgcolor       textcolor
#define clrscr        ClearBitmap

// Array for printing to VRAM
extern unsigned char vram_list[256];

// Set VRAM address (as function of txtX/txtY)
void __fastcall__ SetVramAddr(void);

// Hires gfx functions (see blitXXX.s)
void __fastcall__ BlitCharmap(void);

// Reading files from ROM (see files.c)
void DirList(void);
unsigned char* FileRead(const char* filename);
