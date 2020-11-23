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
#define SCREENRAM  (0x0950) // 0940-0cff (text mode data)
#define BITMAPRAM1 (0x7010) // 7010-8f50 (bitmap frame 1)
#define BITMAPRAM2 (0xa010) // a010-bf50 (bitmap frame 2)
#define CHARMAPRAM (0x7000) // 7000-7fff (character map)
#define CHARSETRAM (0xa000) // a000-a3ff (character set)
#define CHARATRRAM (0xa400) // a400-a47f (character col atr)
#define CHARFLGRAM (0xa480) // a480-a4ff (character flags)
#define RMTPLAYER  (0x90e0) // 90e0-9a4d (RMT music player; JSR to 0x9400) (overlaps with unused part of PMGRAM)
#define PMGRAM     (0x9800) // 9800-9fff (player missile memory)
#define MUSICRAM   (0xc000) // c000-cbff (RMT sound track)

// Character Mode
#define CHR_COLS 40
#define CHR_ROWS 25

// Bitmap Mode (INP)
#define BMP_COLS 160
#define BMP_ROWS 200
#define BMP_PALETTE 16

// Bitmap Palette
#define BLACK  	0
#define DBLUE   2
#define DGREEN  3
#define BROWN 	4
#define RED    	5
#define ORANGE 	7
#define GREY   	9
#define WHITE  	9
#define BLUE   	10
#define CYAN   	10
#define GREEN  	11
#define YELLOW 	15

// Clock
#define TCK_PER_SEC	CLK_TCK

// Using xBios for File Management
void DirList(void);
unsigned char FileOpen(const char* fname);
void FileRead(void* buf, unsigned int len);

// Frame/Sprite Flicker Routine (see DLI.s)
void BitmapDLIST(void);
void CharmapDLIST(void);
void StartDLI(void);
extern unsigned char bitmapDLI;
extern unsigned char charmapDLI;
extern unsigned char spriteDLI;
extern unsigned char chrPalette[];
extern unsigned char bmpPalette[];
extern unsigned char frameBlending;

// Gfx functions (see scroll.s)
void __fastcall__ Scroll(void);
