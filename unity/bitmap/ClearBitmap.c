/*
 * Copyright (c) 2018 Anthony Beaucamp.
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

// Clear entire bitmap screen
void ClearBitmap(void)
{
#if defined __APPLE2__
    // clear main and aux screen memory	
  #if defined __DHR__	
	*dhraux = 0;
    bzero((char *)BITMAPRAM, 8192);
	*dhrmain = 0;
  #endif
    bzero((char *)BITMAPRAM, 8192);
	
#elif defined __ATARI__
	// clear both frames
	bzero((char*)BITMAPRAM1, 8000);
  #ifdef __ATARIXL__
	bzero((char*)BITMAPRAM2, 8000);
  #endif
	
#elif defined __CBM__
	bzero((char*)BITMAPRAM, 8000);
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);
	
#elif defined __LYNX__
	unsigned char i;
	unsigned int addr = BITMAPRAM;
	memset((char*)addr, 0xff, 8364); 
	for (i=0; i<102; i++) { 
		POKE((char*)addr, 0x52); addr+=81;
		POKE((char*)addr, 0x00); addr+=1;
	}
	UpdateDisplay();

#elif defined __NES__
	// Assign default palette and reset VRAM
	pal_bg(palBG);	
    FillVram(0);
	
#elif defined __ORIC__
	// reset pixels and set AIC Paper/Ink
	unsigned char y;
	memset((char*)BITMAPRAM, 64, 8000);	
    for (y=0; y<200; y++)
		POKE((char*)BITMAPRAM+y*40, (y&1) ? 6 : 3);	
#endif
}
