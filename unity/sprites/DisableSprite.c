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

#if (defined __APPLE2__) || (defined __ORIC__)
  void RestoreSprBG(unsigned char index); // See SetSprite
  void ClearSprite(signed char index) {
	if (sprBG[index]) {
		if (sprDrawn[index]) 
			RestoreSprBG(index); 
		free(sprBG[index]);
		sprBG[index] = 0;
	}
  }
#elif (defined __ATARI__)
  extern unsigned char posPM0[];	
#endif

void DisableSprite(signed char index)
{
	// Switch single sprite off
	if (index >= 0) {	
	  #if defined __CBM__
		// Set bit in sprite register
		POKE(53269, PEEK(53269) & ~(1 << index));
	  #elif defined __ATARI__
		// Was sprite drawn?
		if (sprDrawn[index]) {
			// Clear sprite DLI and slot
			bzero(&posPM0[sprBegDLI[index]], sprDLIs);
			sprCollide[index] = 0;
			sprDrawn[index] = 0;
		}
		sprBank[index/4u] &= ~sprMask[index];
	  #else
	   #if (defined __APPLE2__) || (defined __ORIC__)
		// Soft sprites: Restore background if neccessary
		ClearSprite(index);
	   #elif defined __NES__
		// Hide metasprite
		oam_set(index<<4); oam_hide(4);
	   #endif
		// Clear drawn flag
		sprDrawn[index] = 0;
	  #endif
	// Switch all sprites off
	} else {
	  #if defined __CBM__
		// Reset sprite register
		POKE(53269, 0);
	  #elif defined __ATARI__
		// Clear sprite DLIs, banks and PMG memory
		bzero(sprCollide, SPRITE_NUM);
		bzero(sprDrawn, SPRITE_NUM);
		bzero(sprBank, BANK_NUM);
		bzero(posPM0, 100);
	  #elif defined __LYNX__
		// Clear sprite slots
		bzero(sprDrawn, SPRITE_NUM);
	  #elif defined __NES__
		// Clear sprite slots
		bzero(sprDrawn, SPRITE_NUM);
		oam_clear();
	  #else
		// Soft sprites: Restore background if necessary
		for (index=0; index<SPRITE_NUM; index++) {
			ClearSprite(index);
			sprDrawn[index] = 0;
		}
	  #endif
	}
}
