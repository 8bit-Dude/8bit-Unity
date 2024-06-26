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

// Scroll sprites: 0 = no, 1 = yes
unsigned char sprScroll = 1;

void EnableSprite(unsigned char index)
{	
#if (defined __APPLE2__) || (defined __ORIC__)
	// Allocate memory for background
	if (!sprBG[index]) {
	  #if defined __APPLE2__
		sprBG[index] = (unsigned char*)malloc(2*SPRITEHEIGHT);	// 2 bytes per line (4 bytes between AUX/MAIN for Apple DHR)
	  #else
		sprBG[index] = (unsigned char*)malloc(4*SPRITEHEIGHT);	// 4 bytes per line (2 atrributes + 12 pixels)
	  #endif
	}

#elif defined __ATARI__
	// Set sprite bank and collider
	sprBank[index/4u] |= sprMask[index];
	sprCollide[index] = collideEnabled;

#elif defined __CBM__
	// Set sprite bits
	POKE(53269, PEEK(53269) |  (1 << index));
#endif

	// Set scroll state
	if (sprScroll) {
		sprFlags[index] = SPR_SCROLL;
	}
}
