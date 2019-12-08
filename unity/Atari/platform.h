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
#define DLIST	   (0x6600) // 6600-66df (DLI list)
							// 6f50-700a (START/STOP routines for bitmap mode)
#define RMTPLAYER  (0x66e0) // 66e0-6f4d (RMT music player; JSR to 0x6A00)
#define BITMAPRAM1 (0x7010) // 7010-8f50 (bitmap frame 1)
#define MUSICRAM   (0x9000) // 9000-96ff (RMT sound track)
#define SPRITERAM  (0x9700)	// 9700-9aff (sprite data loaded here, overlaps with unused part of PMGRAM)
#define PMGRAM     (0x9800) // 9800-9fff (player missile memory)
#define PALETTERAM (0xa000) // a000-a003 (palette data)
#define BITMAPRAM2 (0xa010) // a010-bf50 (bitmap frame 2)

// External Routines/Variables 
#define BLENDTOG   (0x6f50) // Toggle for frame blending ON/OFF (see DLI.a65)
#define STARTBMP   (0x6f5b) // Start Bitmap routine (see DLI.a65)
#define STOPBMP    (0x6fa1) // Stop Bitmap routine (see DLI.a65)

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
#define GREEN  	11
#define YELLOW 	15

// Clock
#define TCK_PER_SEC	CLK_TCK
