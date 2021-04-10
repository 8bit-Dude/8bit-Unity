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
 
// Memory Map
#define VIDEOBANK  3
#define SCREENLOC  0
#define BITMAPLOC  8
#define SPRITELOC  32
#define MUSICRAM   (0x0800) 								 // 0800-17FF (SID sound track: ALSO EDIT SID.S WHEN CHANGING THIS VALUE!)
#define SCREENRAM  (VIDEOBANK * 0x4000 + SCREENLOC * 0x0400) // C000-C3FF (screen data)
#define COLORRAM   (0xd800) 								 // D800-DBFF (color data) *fixed location*
#define BITMAPRAM  (VIDEOBANK * 0x4000 + BITMAPLOC * 0x0400) // E000-FF40 (bitmap data)
#define CHARSETRAM (BITMAPRAM)						 		 // E000-E800 (charset+font data)
#define SPRITEPTR  (SCREENRAM + 0x03f8)						 // C3F8-???? (sprite control registers)
#define SPRITERAM  (VIDEOBANK * 0x4000 + SPRITELOC * 0x0040) // C800-D800 (sprites.dat loaded here)

// Character Mode
#define TXT_COLS   40
#define TXT_ROWS   25
#define FONT_HEIGHT 8	

// Charmap Mode
#define CMP_COLS 40
#define CMP_ROWS 25	

// Bitmap Mode (Multi-Color)
#define BMP_COLS 160
#define BMP_ROWS 200
#define BMP_PALETTE 16
	
// Bitmap Palette
#define BLACK  	0
#define WHITE  	1
#define RED    	2
#define CYAN   	3
#define PURPLE 	4
#define GREEN  	5
#define BLUE   	6
#define YELLOW 	7
#define LBROWN 	8
#define BROWN 	9
#define ORANGE 	10
#define PINK 	10
#define DGREY   11	
#define MGREY 	12
#define GREY    12	
#define LGREEN  13
#define LBLUE   14
#define LGREY   15

// Video State (auto-set by ShowBitmap()/HideBitmap()...)
#define TXT_MODE 0
#define BMP_MODE 1
#define CHR_MODE 2

// Clock
#define TCK_PER_SEC	CLK_TCK

// VIC2 functions
void SetupVIC2(void);
void ResetVIC2(void);

// Rom function: use before/after accessing BITMAPRAM (see ROM.s)
extern void __fastcall__ rom_enable(void);
extern void __fastcall__ rom_disable(void);

// File Management
void DirList(void);

// Gfx functions (see blitCharmap.s)
void __fastcall__ BlitCharmap(void);

// Decrunch data compressed with Exomizer (see decrunch.s)
void Decrunch(unsigned int address);
