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
 
 /*
								Note coordinate restrictions on each platform:
	---------------------------------------------------------------------------------------------------------
	Apple:  X/W must be multiples of 7 (e.g. 0,7,14,21...) |              No restrictions
	Atari:  X/W must be multiples of 4 (e.g. 0,4,8,12...)  |              No restrictions
	C64:    X/W must be multiples of 4 (e.g. 0,4,8,12...)  |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
	Lynx:   X/W must be multiples of 2 (e.g. 0,2,4,6...)   |              No restrictions
	NES:    X/W must be multiples of 8 (e.g. 0,8,16,24...) |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)  (Then divide by 8)
   	Oric:   X/W must be multiples of 6 (e.g. 0,6,12,18...) |  			  No restrictions
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

unsigned char chunkBuf[CHUNKSIZE];
unsigned char *chunkPtr = chunkBuf;

#if (defined __NES__)
 #pragma bss-name(pop)
#endif

void GuruMeditation(unsigned char* souce);

void CheckMemory(void)
{
	// Check memory usage
	if ((unsigned int)chunkPtr > (unsigned int)(chunkBuf+CHUNKSIZE))
		GuruMeditation("Chunks");
}

void FreeChunk(unsigned char* chunk)
{
	unsigned char offset;
	if (chunk == 0) {
		// Return to beginning of stack
		chunkPtr = chunkBuf;
	} else {
		// Pop chunk
		offset = *(unsigned int*)&chunk[4];
		if (chunkPtr != chunk+offset)  // Was this the last chunk?
			memcpy(chunk, chunk+offset, CHUNKSIZE-offset);
		chunkPtr -= offset;
	}
}
