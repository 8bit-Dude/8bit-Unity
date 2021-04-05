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
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __CBM__
  extern unsigned char videoMode;
#endif

// Print numerical value
void PrintNum(unsigned int num)
{
	unsigned char tmp, bckX = txtX;
	unsigned int step = 1;
	while ((step*10)<=num) {
		step *= 10;
	}
	while (step>0) {
		if (num >= step) { 
			tmp = num/step;
			num -= tmp*step;
		} else {
			tmp = 0;
		}
	  #if defined __CBM__
		if (videoMode == CHR_MODE) {
			// Charmap mode
			POKE(SCREENRAM+40*row+col, 208+tmp);
			POKE(COLORRAM+40*row+col, inkColor);
			col++;
		} else {
			// Bitmap mode	
			PrintChr(&charDigit[tmp*3]);
			txtX++;
		}
	  #else
		PrintChr(&charDigit[tmp*3]);
		txtX++;
	  #endif
		step /= 10u;
	}
	txtX = bckX;
#if defined __LYNX__
	if (autoRefresh) { UpdateDisplay(); }
#endif	
}
