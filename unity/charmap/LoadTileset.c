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
#endif

// Load tileset from file
void LoadTileset(char *filename, unsigned int n) 
{
	// Assign memory and ZP pointer
	unsigned int size = n*TILE_WIDTH*TILE_HEIGHT;	
#if defined(__ATARI__)	
	unsigned int i = 0;
	unsigned char c;
#endif
#ifndef __NES__
	tilesetData = malloc(size);
#endif
	
#if defined(__APPLE2__) || defined(__ATARI__) || defined(__CBM__) || defined(__NES__) || defined(__ORIC__)
	if (FileOpen(filename)) {
		FileRead(tilesetData, size);
		FileClose();
	}	
#elif defined __LYNX__
	if (FileOpen(filename))	{ // Data is immediately loaded into BITMAP memory on open
		memcpy(tilesetData, (char*)BITMAPRAM, size);
	}
#endif

	// Assign Attributes
#if defined(__ATARI__)	
	while (i < size) {
		c = tilesetData[i];
		tilesetData[i++] += charatrData[c];
	}
#endif
	
	// Allocate buffer for tile to char conversion
	decodeHeight = screenHeight+TILE_HEIGHT;
	decodeWidth  = screenWidth+TILE_WIDTH;
	tileRows = decodeHeight/TILE_HEIGHT;
	tileCols = decodeWidth/TILE_WIDTH;
#ifndef __NES__	
	decodeData = malloc(decodeWidth*decodeHeight);
#endif
}
