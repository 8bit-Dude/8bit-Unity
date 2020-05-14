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

#if (defined __APPLE2__) || (defined __ATMOS__)
  void __fastcall__ Blit(void);
#endif

void LoadFragment(unsigned char** fragment, char *filename, unsigned char w, unsigned char h) 
{
#if defined __LYNX__
	// Lynx: data is already in RO segment
	*fragment = FileRead(filename);
#else
	// Other: Need to load data from disk
	unsigned int size;
  #ifndef __ATMOS__
	FILE* fp;
  #endif
	
	// Compute Byte Size	
  #if defined __APPLE2__
	size = (w*h*4)/7;
  #elif defined __ATARI__
	size = (w*h*2)/4;
  #elif defined __ATMOS__
	size = (w*h*2)/3;
  #endif

	// Allocate fragment memory	
	*fragment = (unsigned char*)malloc(size);

	// Read Fragment File
  #if defined __ATMOS__
	FileRead(filename, *fragment);
  #else
	fp = fopen(filename, "rb");
	fread(*fragment, 1, size, fp);
  #endif
#endif
}

void GetFragment(unsigned char** fragment, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
#if defined __APPLE2__
	unsigned int size = (w*h*4)/7;
#elif defined __ATARI__
	unsigned char i, bytes = w/4;
	unsigned int size = (w*h*2)/4;
	unsigned int addr;
#elif defined __ATMOS__
	unsigned int size = (w*h*2)/3;
	unsigned int addr;
#elif defined __LYNX__
	unsigned char i, bytes = w/2;
	unsigned int size = (w*h)/2;
	unsigned int addr;
#endif

	// Allocate fragment memory	
	*fragment = (unsigned char*)malloc(size);

#if defined __APPLE2__
	// Blit data from DHR memory
	POKE(0xE3, 2*w/7);		// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xEB, h);			// Number of lines
	POKE(0xEC, 2*x/7);		// DHR Offset X
	POKE(0xED, y);			// DHR Offset Y
	POKEW(0xEE, *fragment);	// Address of Output
	POKEW(0xFA, 0);			// Address of Input
	Blit();
#elif defined __ATARI__
	// Copy data from double buffer
	addr = *fragment;
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM1+(y+i)*40+x/4), bytes);
		addr += bytes;
	}
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM2+(y+i)*40+x/4), bytes);
		addr += bytes;
	}
#elif defined __ATMOS__
	// Blit data from Graphic memory
	addr = BITMAPRAM + y*80 + x/3 + 1;
	POKE(0xb0, 2*h); 			// Number of lines
	POKE(0xb1, w/3);			// Bytes per line
	POKEW(0xb2, addr-1);		// Address of source (-1)
	POKEW(0xb4, *fragment-1);	// Address of target (-1)
	POKE(0xb6, 40); 			// Offset between source lines
	POKE(0xb7, w/3); 			// Offset between target lines
	Blit();	
#elif defined __LYNX__
	// Copy data to graphic memory
	addr = *fragment;	
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM+(y+i)*82+x/2+1), bytes);
		addr += bytes;
	}
#endif
}

void SetFragment(unsigned char* fragment, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
#if defined __APPLE2__
	// Blit data to DHR memory
	POKE(0xE3, 2*w/7);		// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xEB, h);			// Number of lines
	POKE(0xEC, 2*x/7);		// DHR Offset X
	POKE(0xED, y);			// DHR Offset Y
	POKEW(0xEE, 0);			// Address for copying DHR > Output
	POKEW(0xFA, fragment);	// Address for copying Input > DHR
	Blit();
#elif defined __ATARI__
	// Copy data to double buffer
	unsigned char i, bytes = w/4;
	unsigned int addr = fragment;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM1+(y+i)*40+x/4), (char*)addr, bytes);
		addr += bytes;
	}
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM2+(y+i)*40+x/4), (char*)addr, bytes);
		addr += bytes;
	}
#elif defined __ATMOS__
	// Blit data to graphic memory
	unsigned int addr = BITMAPRAM + y*80 + x/3 + 1;
	POKE(0xb0, 2*h); 			// Number of lines
	POKE(0xb1, w/3);			// Bytes per line
	POKEW(0xb2, fragment-1);	// Address of source (-1)
	POKEW(0xb4, addr-1);		// Address of target (-1)
	POKE(0xb6, w/3); 			// Offset between source lines
	POKE(0xb7, 40); 			// Offset between target lines
	Blit();	
#elif defined __LYNX__
	// Copy data to graphic memory
	unsigned char i, bytes = w/2;
	unsigned int addr = fragment;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM+(y+i)*82+x/2+1), (char*)addr, bytes);
		addr += bytes;
	}
#endif
}
