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

// Helper function for soft sprites
#if (defined __APPLE2__) || (defined __ORIC__)
  extern unsigned char sprDrawn[];
  void RestoreSprBG(unsigned char index);
  void HideSprites(void) {
	unsigned char i;
	for (i=0; i<SPRITE_NUM; i++)
		if (sprDrawn[i]) {			
			RestoreSprBG(i);
			sprDrawn[i] = 0;
		}
  }
#endif

void DrawCharmap(unsigned char x, unsigned char y)
{
#ifdef __NES__
	unsigned char i,j;
	unsigned char *ptr;
#endif
	
	// Platform specific handling
#if defined(__APPLE2__)
	x = 2*(x/2u)+1;
  #if defined(__DHR__)
	clk += 20;
  #else
	clk += 10;  
  #endif
#endif

	// Hide soft sprites
#if defined(__APPLE2__) || defined(__ORIC__)
	HideSprites();
#endif

	// Save new coordinates (for scrolling)
	worldX = x; worldY = y;
		
#ifdef __NES__
	// Decode tiles (if necessary)
	ptr = DecodeTiles();

	// Send to VRAM
	txtX = screenCol1; txtY = screenRow1;
	for (j=0; j<screenHeight; j++) {
		SetVramName();
		for (i=0; i<screenWidth; i++)
			SetVramChar(*(ptr+i));
		ptr += decodeWidth;
		txtY++; 
	}
#else
	// Decode tiles (if necessary)
	POKEW(charPtrZP, DecodeTiles());
	
	// Blit decoded map to screen
 #ifndef __APPLE2__
	POKEW(scrPtrZP, (unsigned int)screenData);
  #if defined __ATARI__
	POKEW(charatrDataZP, charatrData);
  #elif defined __CBM__	
	POKEW(charatrDataZP, charatrData);
	POKEW(colPtrZP, colorData);
  #endif
 #endif
	BlitCharmap();
#endif
}
