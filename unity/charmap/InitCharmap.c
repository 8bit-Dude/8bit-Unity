/*
 * Copyright (c) 2020 Anthony Beaucamp.
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

#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
 #pragma bss-name(push, "XRAM")
#endif

// Map size and location properties
unsigned char charmapWidth, charmapHeight;
unsigned char worldWidth, worldHeight, worldX, worldY, worldMaxX, worldMaxY;
unsigned char screenCol1 = 0, screenCol2 = CMP_COLS, screenWidth = CMP_COLS;
unsigned char screenRow1 = 0, screenRow2 = CMP_ROWS, screenHeight = CMP_ROWS;
unsigned char lineBlock;

// Decoding properties
unsigned char blockWidth, lineWidth, decodeWidth, decodeHeight;
unsigned char tileX, tileY, tileCols, tileRows;
unsigned char scrollCols, scrollRows, scrollDirX, scrollDirY;
  
// Pointers to various data sets
#if defined(__APPLE2__)
  unsigned char *charmapData;
 #if defined __DHR__	
  unsigned char  charset[0x1080];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x1000);
 #else
  unsigned char  charset[0x0880];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0800);
 #endif
#elif defined(__ATARI__) 
  unsigned char *charmapData = (char*)(CHARMAPRAM);
  unsigned char *charatrData = (char*)(CHARATRRAM);
  unsigned char *charflagData = (char*)(CHARATRRAM+0x80);
  unsigned char *charsetData = (char*)(CHARSETRAM);
#elif defined(__CBM__)   
  unsigned char *charmapData;
  unsigned char  charatr[0x0100];			
  unsigned char *charatrData = (char*)(charatr+0x0000);
  unsigned char *charflagData = (char*)(charatr+0x0080);
  unsigned char *charsetData = (char*)(CHARSETRAM);
  unsigned char *colorData;
#elif defined(__LYNX__)
  unsigned char *charmapData;
  unsigned char  charset[0x0480];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0400);
#elif defined(__NES__)
  unsigned char  charmapData[CHARMAPRAM];
  unsigned char  charflagData[CHARFLAGRAM];  
#elif defined(__ORIC__)
  unsigned char *charmapData;
  unsigned char  charset[0x0480];
  unsigned char *charsetData = (char*)(charset+0x0000);
  unsigned char *charflagData = (char*)(charset+0x0400);
#endif

#if defined(__NES__)
  unsigned char  decodeData[DECODERAM];  
  unsigned char  tileset[128*TILE_WIDTH*TILE_HEIGHT];  
  unsigned char *tilesetData = tileset;  
#else
  unsigned char *decodeData;
  unsigned char *screenData;
  unsigned char *tilesetData;
#endif

#if defined __NES__
 #pragma bss-name(pop)
#endif

// Initialize Charmap Mode
void InitCharmap(unsigned char col1, unsigned char col2, unsigned char row1, unsigned char row2) 
{
	// Define rendering window
	lineWidth = 40;
	screenCol1 = col1; screenCol2 = col2;
	screenRow1 = row1; screenRow2 = row2;
	screenWidth = col2-col1; screenHeight = row2-row1;	
	lineBlock = screenWidth * CHAR_WIDTH;
#if (defined __ATARI__) || (defined __CBM__)
	screenData = (char*)(SCREENRAM + screenRow1*LINE_SIZE + screenCol1*CHAR_WIDTH);
#elif (defined __LYNX__) || (defined __ORIC__)
	screenData = (char*)(BITMAPRAM+1 + screenRow1*ROW_SIZE + screenCol1*CHAR_WIDTH);
#endif

#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	InitBitmap();
#elif defined __ATARI__	
	// Init Graphic Mode
	InitBitmap();
  #ifdef __ATARIXL__
	doubleBuffer = 0;	// Disable Frame Blending
  #else
	POKE(0x02F4, 0xA0);	// Set Address of Char. Gen.
  #endif
  
	// Charmap/Bitmap transition params	
	chrRows = row2-1;
	bmpRows = chrRows + 8*(CMP_ROWS-row2) + 2;
	bmpAddr = BITMAPRAM1 + row2*(8*40);
#elif defined __CBM__
	// Charmap/Bitmap transition params
	rasterLine = 50 + row2*8;
	colorData  = (char*)(COLORRAM + screenRow1*LINE_SIZE + screenCol1*CHAR_WIDTH);
#endif
}
