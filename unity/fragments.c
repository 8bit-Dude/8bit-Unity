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

// Debugging flags
//#define DEBUG_FRAG

void __fastcall__ Blit(void);

void LoadFragment(unsigned char** fragment, char *filename, unsigned char width, unsigned char height) 
{
	// Compute Byte Size
	FILE* fp;
	unsigned int size;
#if defined __APPLE2__
	size = (height*width*4)/7;
#elif defined __ATARI__
	size = (height*width*2)/4;
#endif
	// Allocate fragment memory	
	*fragment = (unsigned char*)malloc(size);
	
	// Open Fragment File
	fp = fopen(filename, "rb");
	fread(*fragment, 1, size, fp);
}

void GetFragment(unsigned char** fragment, unsigned char xBeg, unsigned char yBeg, unsigned char xEnd, unsigned char yEnd)
{
#if defined __APPLE2__
	unsigned int size = ((yEnd-yBeg)*(xEnd-xBeg)*4)/7;
#elif defined __ATARI__
	unsigned char i, bytes = (xEnd-xBeg)/4;
	unsigned int size = ((yEnd-yBeg)*(xEnd-xBeg)*2)/4;
	unsigned int addr;
#endif

	// Allocate fragment memory	
	*fragment = (unsigned char*)malloc(size);

#if defined __APPLE2__
	// Blit data from DHR memory
	POKE(0xE3, 2*(xEnd-xBeg)/7);// Number of bytes per block (x2 for MAIN/AUX)	
	POKE(0xEB, yEnd-yBeg);		// Number of blocks
	POKE(0xEC, (2*xBeg)/7);		// DHR Offset X
	POKE(0xED, yBeg);			// DHR Offset Y
	POKEW(0xEE, fragment);		// Address of Output
	POKEW(0xFA, 0);				// Address of Input
	Blit();
#elif defined __ATARI__
	// Copy data from double buffer
	addr = *fragment;
	for (i=yBeg; i<yEnd; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM1+i*40+xBeg/4), bytes);
		addr += bytes;
	}
	for (i=yBeg; i<yEnd; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM2+i*40+xBeg/4), bytes);
		addr += bytes;
	}
#endif
}

void SetFragment(unsigned char* fragment, unsigned char xBeg, unsigned char yBeg, unsigned char xEnd, unsigned char yEnd)
{
#if defined __APPLE2__
	// Blit data to DHR memory
	POKE(0xE3, 2*(xEnd-xBeg)/7);// Number of bytes per block (x2 for MAIN/AUX)	
	POKE(0xEB, yEnd-yBeg);		// Number of blocks
	POKE(0xEC, (2*xBeg)/7);		// DHR Offset X
	POKE(0xED, yBeg);			// DHR Offset Y
	POKEW(0xEE, 0);				// Address of Output
	POKEW(0xFA, fragment);		// Address of Input
	Blit();
#elif defined __ATARI__
	unsigned char i, bytes = (xEnd-xBeg)/4;
	unsigned int addr = fragment;
	
	// Copy data to double buffer
	for (i=yBeg; i<yEnd; ++i) {
		memcpy((char*)(BITMAPRAM1+i*40+xBeg/4), (char*)addr, bytes);
		addr += bytes;
	}
	for (i=yBeg; i<yEnd; ++i) {
		memcpy((char*)(BITMAPRAM2+i*40+xBeg/4), (char*)addr, bytes);
		addr += bytes;
	}
#endif

#if defined DEBUG_FRAG	// Add visual markers around fragments
	LocatePixel(xBeg, yBeg); SetPixel(BLACK);
	LocatePixel(xEnd, yEnd); SetPixel(BLACK);
#endif
}
