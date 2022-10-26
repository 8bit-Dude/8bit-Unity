/*
 * Copyright (c) 2022 Anthony Beaucamp.
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

extern unsigned char chunkBuf[CHUNKSIZE];
extern unsigned char *chunkPtr;
void CheckMemory(void);

unsigned char LoadChunks(unsigned char *chunks[], char *filename) 
{
	unsigned char i, n=0;
	unsigned int offset;

	if (FileOpen(filename)) {
		// Read data from file
	#if defined __DHR__	
		FileRead((unsigned char*)&offset, 2);
		FileRead(chunkPtr, offset);	// Load AUX data 
		MainToAux(chunkPtr, offset);	
		FileRead(chunkPtr, offset);	// Load MAIN data 
	#else
		FileRead(chunkPtr, -1);		
	#endif
		FileClose();

		// Create pointers to individual chunks
		n = chunkPtr[0]; 
		chunkPtr += 1;
		for (i=0; i<n; i++) {
			chunks[i] = chunkPtr;
			chunkPtr += *(unsigned int*)&chunkPtr[4];
		}
		
		// Detect buffer overflow
		CheckMemory();
	}
	
	return n;	
}
