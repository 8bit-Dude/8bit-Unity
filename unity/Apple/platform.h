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
#define BITMAPRAM  (0x2000)
#define MUSICRAM   (0xa800) // A800-AAFF (electric duet track loaded here)
#define SPRITERAM  (0xab00)	// AB00-BEFF (sprites.app loaded here)

// Character Mode
#define CHR_COLS 40
#define CHR_ROWS 24

// Bitmap Mode (DHR)
#define BMP_COLS 140
#define BMP_ROWS 192
#define BMP_PALETTE 16

// Bitmap Palette
#define BLACK   0
#define DBLUE	1
#define DGREEN	2
#define BLUE	3
#define BROWN	4
#define GREY	5
#define GREEN	6
#define LGREEN	7
#define RED 	8
#define PURPLE  9
#define LGREY	10
#define LBLUE	11
#define ORANGE	12
#define PINK	13
#define YELLOW  14
#define WHITE   15

// Double Hi-Res functions (see DHR.c)
extern unsigned char *dhrmain, *dhraux, *dhrptr, dhrpixel;
void SetDHRPointer(unsigned int x, unsigned int y);
void SetDHRColor(unsigned char color);
unsigned char GetDHRColor(void);

// Workaround for missing chardefs
#define CH_DEL  0x08

// Workaround for missing clock (see CLOCK.c)
#define TCK_PER_SEC	59
void tick(void);
clock_t clock(void);
unsigned sleep(unsigned seconds);
extern clock_t clk;	
