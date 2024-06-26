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

unsigned int DecodeTiles(void)
{
#if defined __TILE_NONE__
	blockWidth = charmapWidth;
	return (unsigned int)&charmapData[charmapWidth*worldY + worldX];
#else
	// Decode tilemap to screen buffer
  #if defined __TILE_2X2__
	if (tileX != worldX/2u || tileY != worldY/2u) {
		tileX = worldX/2u; tileY = worldY/2u;
		POKEW(charPtrZP, (unsigned int)&charmapData[charmapWidth*tileY + tileX]);
		POKEW(decPtr1ZP, (unsigned int)&decodeData[0]);
		POKEW(decPtr2ZP, (unsigned int)&decodeData[decodeWidth]);	
		blockWidth = 2*decodeWidth;
		DecodeTiles2x2();
	}
  #endif	
	// Assign offset area of screen buffer
	blockWidth = decodeWidth;
	return (unsigned int)&decodeData[decodeWidth*(worldY&1)+(worldX&1)];	
#endif
}
