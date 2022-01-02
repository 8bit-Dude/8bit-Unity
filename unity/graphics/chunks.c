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

#if (defined __NES__)
 #pragma bss-name(push, "XRAM")
  unsigned char chunkBuf[CHUNKRAM];
  unsigned char* chunkPtr = chunkBuf;
 #pragma bss-name(pop)
#elif (defined __ORIC__)
  unsigned char chunkBuf[512];
#else
  unsigned char chunkBuf[4];
#endif

unsigned int ChunkSize(unsigned char w, unsigned char h)
{
#if defined __APPLE2__
	return 4+(w*h*2)/7u;	// For each group of 7 pixels: 2 bytes in SHR, 4 bytes in DHR (2 in AUX + 2 in MAIN)
#elif defined __ATARI__
  #if defined __ATARIXL__
	return 4+(w*h)/2u;
  #else
	return 4+(w*h)/4u;
  #endif	  
#elif defined __C64__
	return 4+(w*h*10)/32u;
#elif defined __LYNX__
	return 4+(w*h)/2u;
#elif defined __ORIC__
	return 4+(w*h)/6u;
#endif	
}

void LoadChunk(unsigned char** chunk, char *filename) 
{
#if defined __NES__
	// Read whole block to memory
	unsigned int size = FileOpen(filename);
	FileRead(chunkPtr, size);
	*chunk = chunkPtr;
	chunkPtr += size;
	
#elif defined __LYNX__
	// Read whole block to memory
	unsigned int size = FileOpen(filename);
	*chunk = (unsigned char*)malloc(size);
	FileRead((char*)*chunk, size);
	
#elif defined __ORIC__
	// Block read chunk file (TODO: implement header reading, to get rid of chunkBuf)
	unsigned int size;
	if (FileOpen(filename)) {
		size = FileRead(chunkBuf, -1);
		*chunk = (unsigned char*)malloc(size);
		memcpy(*chunk, chunkBuf, size);
	}
	
#elif defined(__ATARI__) || defined(__APPLE2__) || defined(__CBM__)
	unsigned int size;
	if (FileOpen(filename)) {
		// Read header and compute size
		FileRead((char*)chunkBuf, 4);
		size = ChunkSize(chunkBuf[2], chunkBuf[3]);

		// Allocate memory and read data
		*chunk = (unsigned char*)malloc(size);
		memcpy(*chunk, chunkBuf, 4);	
	  #if defined __DHR__ // Load AUX data first
		FileRead((char*)*chunk+4, size-4);
		MainToAux((char*)*chunk+4, size-4);
	  #endif			
		FileRead((char*)*chunk+4, size-4);		
	}
#endif
}

void GetChunk(unsigned char** chunk, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
#if defined __NES__
	// Transfer data from VRAM to XRAM
	unsigned int vaddr;
	unsigned char i;
	*chunk = chunkPtr;	
	POKE(chunkPtr++, x);
	POKE(chunkPtr++, y);
	POKE(chunkPtr++, w);
	POKE(chunkPtr++, h);
	
	// Get names
	x /= 8u; y /= 8u; y += 2;
	h /= 8u; w /= 8u;	
	ppu_off();	
	for (i=0; i<h; ++i) {
		vaddr = NTADR_A(x,y);
		vram_adr(vaddr);
		vram_read(chunkPtr, w);
		chunkPtr += w; y++;
	}	
	ppu_on_all();	

	// Get attributes
	y -= h; // return to top
	x /= 4u; y /= 4u;
	h /= 4u; w /= 4u;	
	h += 1;  w += 1;
	for (i=0; i<h; ++i) {
		memcpy(chunkPtr, &vram_attr[8*y+x], w);
		chunkPtr += w; y += 1;
	}
		
#else	
	// Allocate memory for bitmap chunk
  #ifndef __APPLE2__
	unsigned char i, bytes;
	unsigned int dst;
  #endif
	unsigned int size = ChunkSize(w,h);
	*chunk = (unsigned char*)malloc(size);
	POKE(*chunk+0, x);
	POKE(*chunk+1, y);
	POKE(*chunk+2, w);
	POKE(*chunk+3, h);

  #if defined __APPLE2__
	// Blit data 
	POKE(0xE3, (w*2)/7u);	// Bytes per line (x2 for DHR)	
	POKE(0xCE, h);			// Number of lines (from screen)
	POKE(0xEB, h);			// Number of lines (to screen)
	POKE(0xEC, (x*2)/7u);	// Hires Offset X
	POKE(0xED, y);			// Hires Offset Y
	POKEW(0xEE, *chunk+4);	// Address of Output
	POKEW(0xFA, 0);			// Address of Input
	BlitSprite();
  
  #elif defined __ATARI__
	bytes = w/4u;
	dst = *chunk+4;
	for (i=0; i<h; ++i) {
		memcpy((char*)dst, (char*)(BITMAPRAM1+(y+i)*40u+x/4u), bytes);
		dst += bytes;
	}
   #ifdef __ATARIXL__	
	for (i=0; i<h; ++i) {
		memcpy((char*)dst, (char*)(BITMAPRAM2+(y+i)*40u+x/4u), bytes);
		dst += bytes;
	}
   #endif
  #elif defined __ORIC__
	dst = BITMAPRAM + y*40 + x/6u;
	POKE(0xb0, h); 			// Number of lines
	POKE(0xb1, w/6u);		// Bytes per line
	POKEW(0xb2, dst-1);		// Address of source (-1)
	POKEW(0xb4, *chunk+3);	// Address of target (-1)
	POKE(0xb6, 40); 		// Offset between source lines
	POKE(0xb7, w/6u); 		// Offset between target lines
	BlitSprite();
	
  #elif defined __C64__
	bytes = (w*8)/4u;
	dst = *chunk+4;	
	rom_disable();
	for (i=0; i<h; i+=8) {
		memcpy((char*)dst, (char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), bytes);
		dst += bytes;
	}
	rom_enable();
	bytes = w/4u;
	for (i=0; i<h; i+=8) {
		memcpy((char*)dst, (char*)(SCREENRAM+40*(y+i)/8u+x/4u), bytes);
		dst += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)dst, (char*)(COLORRAM+40*(y+i)/8u+x/4u), bytes);
		dst += bytes;
	}
	
  #elif defined __LYNX__
	bytes = w/2u;
	dst = *chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)dst, (char*)(BITMAPRAM+(y+i)*82+x/2u+1), bytes);
		dst += bytes;
	}
  #endif	
#endif
}

void SetChunk(unsigned char* chunk, unsigned char x, unsigned char y)
{
#if defined __APPLE2__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	POKE(0xE3, (w*2)/7u);	// Bytes per line (x2 for MAIN/AUX)	
	POKE(0xCE, h);			// Number of lines (from screen)
	POKE(0xEB, h);			// Number of lines (to screen)
	POKE(0xEC, (x*2)/7u);	// Hires Offset X
	POKE(0xED, y);			// Hires Offset Y
	POKEW(0xEE, 0);			// Address for copying Hires > Output
	POKEW(0xFA, chunk+4);	// Address for copying Input > Hires
	BlitSprite();
	
#elif defined __ATARI__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char i, bytes = w/4u;
	unsigned int src = chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM1+(y+i)*40+x/4u), (char*)src, bytes);
		src += bytes;
	}
  #ifdef __ATARIXL__
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM2+(y+i)*40+x/4u), (char*)src, bytes);
		src += bytes;
	}
  #endif
#elif defined __ORIC__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned int dst = BITMAPRAM + y*40u + x/6u;
	POKE(0xb0, h); 			// Number of lines
	POKE(0xb1, w/6u);		// Bytes per line
	POKEW(0xb2, chunk+3);	// Address of source (-1)
	POKEW(0xb4, dst-1);		// Address of target (-1)
	POKE(0xb6, w/6u); 		// Offset between source lines
	POKE(0xb7, 40); 		// Offset between target lines
	BlitSprite();	
	
#elif defined __C64__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char i, bytes;
	unsigned int src = chunk+4;	
	bytes = (w*8)/4u;
	rom_disable();
	for (i=0; i<h; i+=8) {
		memcpy((char*)(BITMAPRAM+40*((y+i)&248)+((y+i)&7)+((x*2)&504)), (char*)src, bytes);
		src += bytes;
	}
	rom_enable();
	bytes = w/4u;
	for (i=0; i<h; i+=8) {
		memcpy((char*)(SCREENRAM+40*(y+i)/8u+x/4u), (char*)src, bytes);
		src += bytes;
	}
	for (i=0; i<h; i+=8) {
		memcpy((char*)(COLORRAM+40*(y+i)/8u+x/4u), (char*)src, bytes);
		src += bytes;
	}
	
#elif defined __LYNX__
	unsigned char w = chunk[2];
	unsigned char h = chunk[3];
	unsigned char i, bytes = w/2u;
	unsigned int src = chunk+4;	
	for (i=0; i<h; ++i) {
		memcpy((char*)(BITMAPRAM+(y+i)*82+x/2u+1), (char*)src, bytes);
		src += bytes;
	}
	UpdateDisplay();
	
#elif defined __NES__
	unsigned char i, j, *ptr = chunk+2;
	unsigned char w = (*ptr++/8u);
	unsigned char h = (*ptr++/8u);

	// Set names
	txtX = x/8u; txtY = y/8u;
	for (i=0; i<h; ++i) {
		SetVramName();
		for (j=0; j<w; j++)
			SetVramChar(*ptr++);	
		txtY++;
	}	
	
	// Set attributes
 	txtY = y/8u;
	h /= 4u; w /= 4u;	
	h += 1;  w += 1;
	for (i=0; i<h; ++i) {
		SetVramAttr();
		for (j=0; j<w; j++) {
			vram_attr[vram_attr_index++] = *ptr;
			SetVramChar(*ptr++); 
		}
		txtY += 4;		
	}
	UpdateDisplay();
#endif
}
