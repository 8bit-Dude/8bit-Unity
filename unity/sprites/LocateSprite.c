/*
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
 */

#include "unity.h"

#ifdef __APPLE2__
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

#if defined __CBM__
  unsigned int spriteX;
  unsigned char spriteY;	
#else
  unsigned char spriteX,spriteY;
#endif

void LocateSprite(unsigned int x, unsigned int y)
{
// This function maps sprite coordinates from a 320x200 screen definition
// It can be by-passed by assigning spriteX, spriteY directly in your code
#if defined __APPLE2__
	spriteX = (x*35)/80u;	// 140x192
	spriteY = (y*24)/25u;
#elif defined __ATARI__
	spriteX = x/2u + 42;	// 160x200
	spriteY = y;
#elif defined __CBM__
	spriteX = x;			// 320x200
	spriteY = y;
#elif defined __LYNX__
	spriteX = x/2u;			// 160x102
	spriteY = (y*51)/100u;
#elif defined __ORIC__
	spriteX = x/4u;			//  80x200	
	spriteY = y;
#elif defined __NES__
	spriteX = (x*4)/5u;		// 256x192
	spriteY = (y*24)/25u+24;
#endif
}
