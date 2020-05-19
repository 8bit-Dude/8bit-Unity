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
	Atari:  X/W must be multiples of 4 (e.g. 0,4,8,12... ) |              No restrictions
	C64:    X/W must be multiples of 4 (e.g. 0,4,8,12... ) |  Y/H must be multiples of 8  (e.g. 0,8,16,24...)
	Lynx:   X/W must be multiples of 2 (e.g. 0,2,4,6... )  |              No restrictions
   	Oric:   X/W must be multiples of 6 (e.g. 0,6,12,18...) |  			  No restrictions
 */
 
#include "unity.h"

#if (defined __APPLE2__) || (defined __ORIC__)
  void __fastcall__ Blit(void);
#endif

#if (defined __ORIC__)
  unsigned char buffer[512];
#else
  unsigned char buffer[6];
#endif

unsigned int ChunkSize(unsigned char w, unsigned char h)
{
#if defined __APPLE2__
	return 4+(w*h*4)/7;
#elif defined __ATARI__
	return 4+(w*h*2)/4;
#elif defined __ORIC__
	return 4+(w*h)/6;
#elif defined __C64__
	return 4+(w*h*10)/32;
#elif defined __LYNX__
	return 4+(w*h)/2;
#endif	
}

void LoadChunk(unsigned char** chunk, char *filename) 
{
#if defined __LYNX__
	// Lynx: data is already in RO segment
	*chunk = FileRead(filename);
#elif defined __ORIC__
	// Block read chunk file (TODO: implement header reading, to get rid of buffer)
	unsigned int size;
	size = FileRead(filename, buffer);
	*chunk = (unsigned char*)malloc(size);
	memcpy(*chunk, buffer, size);
#else 
	// Read header into buffer
	unsigned int size;
	FILE* fp = fopen(filename, "rb");
	fread(buffer, 1, 4, fp);
	
	// Compute chunk size and allocate memory
	size = ChunkSize(buffer[2], buffer[3]);
	*chunk = (unsigned char*)malloc(size);
	memcpy(*chunk, buffer, 4);	
	fread(*chunk+4, 1, size-4, fp);
	fclose(fp);
#endif
}

void GetChunk(unsigned char** chunk, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
	// Allocate memory for bitmap chunk
#if (defined __ATARI__) || (defined __C64__) || (defined __LYNX__) || (defined __ORIC__)
	unsigned char i, bytes;
	unsigned int addr;
#endif
	unsigned int size = ChunkSize(w,h);
	*chunk = (unsigned char*)malloc(size);
	POKE(*chunk+0, x);
	POKE(*chunk+1, y);
	POKE(*chunk+2, w);
	POKE(*chunk+3, h);

#if defined __APPLE2__
	// Blit data from DHR memory
	POKE(0xE3, 2*w/7);		// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xEB, h);			// Number of lines
	POKE(0xEC, 2*x/7);		// DHR Offset X
	POKE(0xED, y);			// DHR Offset Y
	POKEW(0xEE, *chunk+4);	// Address of Output
	POKEW(0xFA, 0);			// Address of Input
	Blit();
	
#elif defined __ATARI__
	// Copy data from double buffer
	bytes = w/4;
	addr = *chunk+4;
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM1+(y+i)*40+x/4), bytes);
		addr += bytes;
	}
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM2+(y+i)*40+x/4), bytes);
		addr += bytes;
	}
#elif defined __ORIC__
	// Blit data from bitmap memory
	addr = BITMAPRAM + y*40 + x/6;
	POKE(0xb0, h); 			// Number of lines
	POKE(0xb1, w/6);		// Bytes per line
	POKEW(0xb2, addr-1);	// Address of source (-1)
	POKEW(0xb4, *chunk+3);	// Address of target (-1)
	POKE(0xb6, 40); 		// Offset between source lines
	POKE(0xb7, w/6); 		// Offset between target lines
	Blit();	
	
#elif defined __C64__
	// Copy data to bitmap, color and screen memory
	bytes = (w*8)/4;
	addr = *chunk+4;	
	DisableRom();
	for (i=0; i<h; i+=8) {
		memcpy((char*)addr, (char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), bytes);
		addr += bytes;
	}
	bytes = w/4;
	for (i=0; i<h; i+=8) {
		memcpy((char*)addr, (char*)(SCREENRAM+40*(y+i)/8+x/4), bytes);
		addr += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)addr, (char*)(COLORRAM+40*(y+i)/8+x/4), bytes);
		addr += bytes;
	}
	EnableRom();
	
#elif defined __LYNX__
	// Copy data to bitmap memory
	bytes = w/2;
	addr = *chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)addr, (char*)(BITMAPRAM+(y+i)*82+x/2+1), bytes);
		addr += bytes;
	}
#endif
}

void SetChunk(unsigned char* chunk, unsigned char x, unsigned char y)
{
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
#if defined __APPLE2__
	// Blit data to DHR memory
	POKE(0xE3, 2*w/7);		// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xEB, h);			// Number of lines
	POKE(0xEC, 2*x/7);		// DHR Offset X
	POKE(0xED, y);			// DHR Offset Y
	POKEW(0xEE, 0);			// Address for copying DHR > Output
	POKEW(0xFA, chunk+4);	// Address for copying Input > DHR
	Blit();
#elif defined __ATARI__
	// Copy data to double bitmap buffers
	unsigned char i, bytes = w/4;
	unsigned int addr = chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM1+(y+i)*40+x/4), (char*)addr, bytes);
		addr += bytes;
	}
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM2+(y+i)*40+x/4), (char*)addr, bytes);
		addr += bytes;
	}
#elif defined __ORIC__
	// Blit data to bitmap memory
	unsigned int addr = BITMAPRAM + y*40 + x/6;
	POKE(0xb0, h); 			// Number of lines
	POKE(0xb1, w/6);		// Bytes per line
	POKEW(0xb2, chunk+3);	// Address of source (-1)
	POKEW(0xb4, addr-1);	// Address of target (-1)
	POKE(0xb6, w/6); 		// Offset between source lines
	POKE(0xb7, 40); 		// Offset between target lines
	Blit();	
#elif defined __C64__
	// Copy data to bitmap, color and screen memory
	unsigned char i, bytes;
	unsigned int addr = chunk+4;	
	bytes = (w*8)/4;
	DisableRom();
	for (i=0; i<h; i+=8) {
		memcpy((char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), (char*)addr, bytes);
		addr += bytes;
	}
	bytes = w/4;
	for (i=0; i<h; i+=8) {
		memcpy((char*)(SCREENRAM+40*(y+i)/8+x/4), (char*)addr, bytes);
		addr += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)(COLORRAM+40*(y+i)/8+x/4), (char*)addr, bytes);
		addr += bytes;
	}
	EnableRom();
#elif defined __LYNX__
	// Copy data to bitmap memory
	unsigned char i, bytes = w/2;
	unsigned int addr = chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM+(y+i)*82+x/2+1), (char*)addr, bytes);
		addr += bytes;
	}
	// Auto-refresh screen
	UpdateDisplay();
#endif
}
